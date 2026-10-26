.global sys_exit
.type sys_exit, @function
.global proc_exit
.type proc_exit, @function
.extern tss
sys_exit:
	mov tss+4(%rip), %rsp

	call proc_exit
	int $32

.global syscall_handler
.type syscall_handler, @function
.extern syscall_table
#define SYSCALL_NUMBER 2
syscall_handler:
	mov %rsp, %r12
	mov tss+4(%rip), %rsp
	push %r12 // rsp
	push %r11 // rflags
	push %rcx // rip

	cmp $SYSCALL_NUMBER, %rax
	jae INVALID_SYSCALL

	call *syscall_table(,%rax, 8)

	pop %rcx
	pop %r11
	pop %rsp
	sysretq
INVALID_SYSCALL:
// TODO
	//call proc_crash
	hlt
	jmp INVALID_SYSCALL
