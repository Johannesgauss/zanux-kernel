#include "syscall.h"
#include "arch/gdt_macros.h"
#include "util/alias.h"
extern void syscall_handler();
void syscall_init()
{
	// Wipes IF bit in RFLAGs
	wrmsr(SF_MASK_ID,  (1 << 9));
	// Set first bit of EFER, which enables syscall/sysret system
	wrmsr(EFER_ID,  rdmsr(EFER_ID) | 1);
	// syscall AND sysret cs ss
	wrmsr(STAR_ID, (uint64_t) GDT_KERNEL_CODE << 0x20 | (uint64_t) GDT_KERNEL_DATA << 0x30);
	// syscall target ip 
	wrmsr(LSTAR_ID, (uint64_t) &syscall_handler);

	// No, these comments weren't AI generated!
}
