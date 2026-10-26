.global retsys 
.type retsys, @function
retsys:
	mov $0, %rax
	mov $0, %rax
	syscall

.global syscall
.type syscall, @function
syscall:
	mov %rdi, %rax
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rcx, %rdx
	mov %r8, %r10
	syscall
	ret
