#include "idt.h"
#include "arch/vmm.h"
#include "mem/pmm.h"
#include "util/alias.h"
#include "arch/gdt_macros.h"
struct arch_proc {
	phy_t cr3;
	struct int_frame intf;
};

// TODO modify name
// btw optimize it as well
inline void arch_proc_init(struct arch_proc *ctx)
{
	virt_array pml4 = (virt_array) pmm_alloc();
	// cpy kernel high half stuff
	for (int i = 0; i < 256; i++) {
		pml4[i] = 0;
	}
	for (int i = 256; i < 512; i++) {
		if (g_pml4[i] & SET_P)
			pml4[i] = g_pml4[i];
	}

	ctx->cr3 = virt_to_phy( pml4);
	asm volatile ("mov %0, %%cr3;" :: "r"(ctx->cr3) :);
	g_pml4 = pml4;
}

inline int in_user_mode(struct int_frame *intf)
{
	return (uint64_t)intf->hw.cs == GDT_USER_CODE;
}
