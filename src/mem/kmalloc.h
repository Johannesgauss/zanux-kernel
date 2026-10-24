#ifndef MEM_KMALLOC_H
#define MEM_KMALLOC_H

#include "../arch/vmm.h"
#include "../util/alias.h"
#include <stddef.h>

virt_t kmalloc(size_t size);
void kfree(virt_t addr);
virt_t vmalloc(size_t size);
void vfree(virt_t addr);

virt_t usr_vmalloc(size_t size);
virt_t usr_kmalloc(size_t size);
void usr_kfree(virt_t addr);
void usr_vfree(virt_t addr);

#endif // MEM_KMALLOC_H
