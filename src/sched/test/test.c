#include "test.h"
#include "util/alias.h"
#include "drivers/gfx/print_text.h"

__attribute__((noipa))
int foo()
{
	for (long i = 0; i < 0xFFFFFFF; i++) { 
	register int64_t rax asm("rax") = 1;
	register int64_t rdi asm("rdi") = 0;
	register const char *rsi asm("rsi") = "I'm foo!\n";
	register umax rdx asm("rdx") = 9;

	asm volatile(
		"syscall"
		: "+r"(rax)
		: "r"(rdi), "r"(rsi), "r"(rdx)
		: "rcx", "r11", "memory"
	);

	}

	register int64_t exit_rax asm("rax") = 0;
	asm volatile("syscall" : "+r"(exit_rax) :: "rcx", "r11", "memory");

	return 0;
}

__attribute__((noipa))
int fizz()
{
	register int64_t rax asm("rax") = 0;

	asm volatile(
		"syscall"
		: "+r"(rax)
		:
		: "rcx", "r11", "memory"
	);

	for (long i = 0; i < 0xFFFFFFF; i++) { 
		asm volatile ("nop");
	}

	register int64_t exit_rax asm("rax") = 0;
	asm volatile("syscall" : "+r"(exit_rax) :: "rcx", "r11", "memory");

	return 0;
}
