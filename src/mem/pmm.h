#ifndef PMM_PMM_H
#define PMM_PMM_H

#include <stdint.h>
#include <stdbool.h>
#include "util/panic.h"

typedef uint64_t phy_t;
typedef void *virt_t;

#define PAGE_SIZE 4096

bool pmm_init();
// first fit
virt_t pmm_alloc();
void pmm_free(virt_t addr);

#define HHDM_OFFSET 0XFFFF800000000000
static inline virt_t phy_to_virt(phy_t phy_addr)
{
	return (virt_t)(phy_addr + HHDM_OFFSET);
}

static inline phy_t virt_to_phy(virt_t virt_addr)
{
	return (phy_t)((uint64_t)virt_addr - HHDM_OFFSET);
}

#endif // PMM_PMM_H
