extern void sys_exit(int retvalue);
extern void print_line(char *line);
#include "syscall.h"
#include "../arch/gdt_macros.h"
#include "../drivers/gfx/print_text.h"

void syscall_handler(int code, int param)
{
	if (code == 0) {
		sys_exit(param);
	}
}
void syscall_init()
{
	// Wipes IF bit in RFLAGs
	wrmsr(SF_MASK_ID,  (1 << 9));
	// Set first bit of EFER, which enables syscall/sysret system
	wrmsr(EFER_ID,  rdmsr(EFER_ID) | 1);
	// syscall cs ss
	wrmsr(STAR_ID, GDT_KERNEL_CODE << 0x10);
	// syscall target ip 
	wrmsr(LSTAR_ID, (uint64_t) &syscall_handler);
}
