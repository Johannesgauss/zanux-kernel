#include "arch/vmm.h"
#include "pmm.h"

// TODO: a better allocation function!

static virt_t g_page_base = (virt_t) 0xFFFF851000000000;
static virt_t g_page_offset = (virt_t) 4096;
//static virt_t g_heap_top = (virt_t) 0xFFFF851000004096; // g_page_base + g_page_offset
static virt_t g_usr_page_base = (virt_t) 0x0000080000000000;
static virt_t g_usr_page_offset = (virt_t) 4096;

virt_t kmalloc(size_t size)
{
	if (size < PAGE_SIZE) {
		virt_t alloc_offset = 0;
		if ((uintptr_t)(g_page_offset + size) < PAGE_SIZE) {
			alloc_offset = g_page_offset;
			g_page_offset += size;
		} else if ((uintptr_t)(g_page_offset + size) >= PAGE_SIZE) {
			g_page_base += PAGE_SIZE;
			vmm_map(g_page_base);
			g_page_offset = (virt_t)size;
		}
		return ((uintptr_t)g_page_base + alloc_offset);
	} else {

		virt_t base_alloc = g_page_base + PAGE_SIZE;
		virt_t i; for (i = 0; i < (virt_t)size; i += PAGE_SIZE) {
			vmm_map((uintptr_t)base_alloc + i);	
		}
		g_page_base += (uintptr_t)i;
		if (size % PAGE_SIZE == 0)
			g_page_base += PAGE_SIZE;
		g_page_offset = (virt_t) (size % PAGE_SIZE);
		return base_alloc;
	}
}

void kfree(virt_t addr)
{
	return;	
}

virt_t vmalloc(size_t size)
{
	return kmalloc(size);
};
void vfree(virt_t addr)
{
	return;
}


virt_t usr_kmalloc(size_t size)
{

	if (size < PAGE_SIZE) {
		virt_t alloc_offset = 0;
		if ((uintptr_t)(g_usr_page_offset + size) < PAGE_SIZE) {
			alloc_offset = g_usr_page_offset;
			g_usr_page_offset += size;
		} else if ((uintptr_t)(g_usr_page_offset + size) >= PAGE_SIZE) {
			g_usr_page_base += PAGE_SIZE;
			vmm_map(g_usr_page_base);
			g_usr_page_offset = (virt_t)size;
		}
		return ((uintptr_t)g_usr_page_base + alloc_offset);
	} else {

		virt_t base_alloc = g_usr_page_base + PAGE_SIZE;
		virt_t i; for (i = 0; i < (virt_t)size; i += PAGE_SIZE) {
			vmm_map((uintptr_t)base_alloc + i);	
		}
		g_usr_page_base += (uintptr_t)i;
		if (size % PAGE_SIZE == 0)
			g_usr_page_base += PAGE_SIZE;
		g_usr_page_offset = (virt_t) (size % PAGE_SIZE);
		return base_alloc;
	}
}

virt_t usr_vmalloc(size_t size)
{
	return usr_kmalloc(size);
}

void usr_kfree(virt_t addr)
{
	return;
}
