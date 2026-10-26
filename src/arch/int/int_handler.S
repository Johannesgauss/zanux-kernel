#define intf_off 0x18
#define pml4_off 0x10
.global int_handler
.type int_handler, @function
.extern tss
#include "arch/gdt_macros.h"
#include "drivers/clk/clk_macros.h"
// was_todo: comment it better!
// Update: Done!
int_handler:
	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %r9
	push %r8
	push %rbp
	push %rdi
	push %rsi
	push %rdx
	push %rcx
	push %rbx
	push %rax

	mov %rsp, %rdi
	call proc_int
	mov %rax, %rbx
	// inside %rbx lies the new process!

	// load the new pml4
	// TODO: this is unnefficient, TLB!
	// I must:
	// skip it if cr3 didn't change
	// PTE_G and PGE
	// PCID
	mov pml4_off(%rax), %rcx
	mov %rcx, %cr3


	mov $PIC1_COMMAND, %rdi
	mov $PIC_EOI, %rsi
	call outb

	add $0xA0, %rsp // d15*8 + 0x28 = 0xA0
	// 0x28 because of hw_frame stuff pushed by machine
	
	// to get rip
	mov intf_off+0x78(%rbx), %rdi
	// to get rflags
	mov intf_off+0x88(%rbx), %rsi
	// to get rsp
	mov intf_off+0x90(%rbx), %rdx

	mov %rsp, tss+4(%rip) 

	push $GDT_USER_DATA
	push %rdx // stack
	//pushfq
	push %rsi
	//push $0x202 // TODO a better way!
	push $GDT_USER_CODE
	push %rdi

	mov intf_off+0x70(%rbx), %r15
	mov intf_off+0x68(%rbx), %r14
	mov intf_off+0x60(%rbx), %r13
	mov intf_off+0x58(%rbx), %r12
	mov intf_off+0x50(%rbx), %r11
	mov intf_off+0x48(%rbx), %r10
	mov intf_off+0x40(%rbx), %r9
	mov intf_off+0x38(%rbx), %r8
	mov intf_off+0x30(%rbx), %rbp
	mov intf_off+0x28(%rbx), %rdi
	mov intf_off+0x20(%rbx), %rsi
	mov intf_off+0x18(%rbx), %rdx
	mov intf_off+0x10(%rbx), %rcx

	mov intf_off+0(%rbx), %rax
	mov intf_off+8(%rbx), %rbx

	iretq
