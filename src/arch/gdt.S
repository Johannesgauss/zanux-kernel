#include "gdt_macros.h"
.global gdt_load
.type gdt_load, @function
gdt_load:
	lgdt (%rdi)
// SI = 1, TI = 0, RPL = 0
	push $GDT_KERNEL_CODE
	lea gdt_load_end(%rip), %rax
	push %rax
	lretq

gdt_load_end:
// SI = 2, TI = 0, RPL = 0
	mov $GDT_KERNEL_DATA, %rax
	mov %rax, %ds
	mov %rax, %ss
	mov %rax, %es
	mov %rax, %fs
	mov %rax, %gs

	// TSS
 	mov $0x28, %rax
	ltr %rax

	ret
