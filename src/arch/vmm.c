#include "mem/pmm.h"
#include "vmm.h"
#include <stdint.h>
#include "util/memory.h"
#include "util/alias.h"

virt_array g_pml4;

static inline void pgtotable(virt_t page)
{
	memset(page, 0, PAGE_SIZE);
}

void vmm_map(virt_t addr)
{
	// 9 9 9 9 12 = 48
	phy_t pml4_offset = (phy_t) ((uint64_t)addr >> (9 + 9 + 9 + 12) & 0x1FF);
	phy_t pdpt_offset = (phy_t) ((uint64_t)addr >> (9 + 9 + 12)) & 0x1FF;
	phy_t pd_offset = (phy_t) ((uint64_t)addr >> (9 + 12)) & 0x1FF;
	phy_t pt_offset = (phy_t) ((uint64_t)addr >> 12) & 0x1FF;

	virt_array pdpt;
	virt_array pd;
	virt_array pt;
	virt_array page;

// TODO: Of course, it's not a great idea all pages having US bit on. Change it later!
	if (!(g_pml4[pml4_offset] & SET_P)) {
		virt_t new_page = pmm_alloc();
		pgtotable(new_page);
		g_pml4[pml4_offset] = virt_to_phy(new_page) | SET_P | SET_RW | SET_US;
	}
	pdpt = phy_to_virt((g_pml4[pml4_offset] & GET_PHY_ADDR));
	if (!(pdpt[pdpt_offset] & SET_P)) {
		virt_t new_page = pmm_alloc();
		pgtotable(new_page);
		pdpt[pdpt_offset] = virt_to_phy(new_page) | SET_P | SET_RW | SET_US;
	}
	pd = phy_to_virt((pdpt[pdpt_offset] & GET_PHY_ADDR));
	if (!(pd[pd_offset] & SET_P)) {
		virt_t new_page = pmm_alloc();
		pgtotable(new_page);
		pd[pd_offset] = virt_to_phy(new_page) | SET_P | SET_RW | SET_US;
	}
	pt = phy_to_virt((pd[pd_offset] & GET_PHY_ADDR));
	if (!(pt[pt_offset] & SET_P)) {
		virt_t new_page = pmm_alloc();
		pgtotable(new_page);
		pt[pt_offset] = virt_to_phy(new_page) | SET_P | SET_RW | SET_US;
	}
	//page = phy_to_virt((pt[pt_offset] & GET_PHY_ADDR));
}

// TODO: copy the cr3 to a new one instead
bool vmm_init()
{
	phy_t cr3;
	asm volatile ("mov %%cr3, %0;" : "=r"(cr3) ::);
	virt_array pml4_old = phy_to_virt((phy_t) cr3 & GET_PHY_ADDR);
	g_pml4 = pmm_alloc();

	for (int i = 256; i < 512; i++) {
		if (pml4_old[i] & SET_P)
			g_pml4[i] = pml4_old[i];
	}
	asm volatile ("mov %0, %%cr3;" :: "r"(virt_to_phy(g_pml4)) :);
	pmm_free(pml4_old);
	return true;
}

// TODO a better vmm_free
void vmm_free(virt_t addr)
{
	phy_t pml4_offset = (phy_t) ((uint64_t)addr >> (9 + 9 + 9 + 12));
	phy_t pdpt_offset = (phy_t) ((uint64_t)addr >> (9 + 9 + 12)) & 0x1FF;
	phy_t pd_offset = (phy_t) ((uint64_t)addr >> (9 + 12)) & 0x1FF;
	phy_t pt_offset = (phy_t) ((uint64_t)addr >> 12) & 0xFFF;

	virt_array pdpt;
	virt_array pd;
	virt_array pt;
	virt_array page;

	if (!(g_pml4[pml4_offset] & SET_P)) goto PANIC;
	pdpt = phy_to_virt((g_pml4[pml4_offset] & GET_PHY_ADDR));

	if (!(pdpt[pdpt_offset] & SET_P)) goto PANIC;
	pd = phy_to_virt((pdpt[pdpt_offset] & GET_PHY_ADDR));

	if (!(pd[pd_offset] & SET_P)) goto PANIC;
	pt = phy_to_virt((pd[pd_offset] & GET_PHY_ADDR));

	if (!(pt[pt_offset] & SET_P)) goto PANIC;
	pmm_free(phy_to_virt(pt[pt_offset] & GET_PHY_ADDR));

	pt[pt_offset] = 0;
	asm volatile ("invlpg (\%0);" :: "r"(addr) : "memory");

	return;
PANIC:
	panic("[ FATAL ] ATTEMPT TO FREE A NON ALLOCATED PAGE!");
}
