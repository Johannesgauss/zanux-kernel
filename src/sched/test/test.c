#include "test.h"
#include "util/alias.h"
#include "drivers/gfx/print_text.h"
extern void retsys();
__attribute__((noipa))
int foo()
{
	retsys();
	//print_line("I'm foo!\n");
	for (long i = 0; i < 0xFFFFFFF; i++) { 
		asm volatile ("nop");
	}
//	while (1) {};
	
}

__attribute__((noipa))
int fizz()
{
	retsys();
	for (long i = 0; i < 0xFFFFFFF; i++) { 
		asm volatile ("nop");
	}
	
	//while (1) {};

}
