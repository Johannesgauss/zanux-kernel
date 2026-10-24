.global retsys 
.type retsys, @function
retsys:
	mov $0, %rdi
	mov $0, %rsi

	syscall

