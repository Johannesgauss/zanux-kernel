#include <stdint.h>
#include <stdbool.h> 
#include "gdt.h"
#include "util/memory.h"
#include "gdt_macros.h"

// TODO document this number better
#define GDT_TSS 0x0089 // 0x0088 | 0x1 (present bit)

struct gdtr_t {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct tss_t {
	uint32_t ign;
	uint64_t rsp0;

	uint16_t boring[45];
	uint16_t useless_io_map;
} __attribute__((packed));
struct tss_t tss;

struct gdtr_t gptr;
gdt_entry_t gpt[7]; // null, 2 kernel, 2 user + 2 halves of tss
extern void gdt_load(struct gdtr_t *gptr);

static gdt_entry_t gdt_entry_init(uint32_t base, uint32_t limit, uint16_t flags)
{
	// I don't know bitmask well enough!
	// Update: got it!
	uint64_t entry = 0;

	entry |= (uint64_t)limit & 0xFFFF;
	entry |= (uint64_t)(limit & 0xF0000) << 0x20;

	entry |= (uint64_t)(base & 0xFFFFFF) << 0x10;
	entry |= (uint64_t)(base & 0xFF000000) << 0x20;

	entry |= (uint64_t)(flags & 0xF0FF) << 0x28;

	return entry;
}

bool gdt_init()
{
	gpt[0] = gdt_entry_init(0, 0, 0);
	gpt[1] = gdt_entry_init(0, 0x000FFFFF, GDT_KERNEL_CODE_FLAGS);
	gpt[2] = gdt_entry_init(0, 0x000FFFFF, GDT_KERNEL_DATA_FLAGS);
	gpt[3] = gdt_entry_init(0, 0x000FFFFF, GDT_USER_DATA_FLAGS);
	gpt[4] = gdt_entry_init(0, 0x000FFFFF, GDT_USER_CODE_FLAGS);

	tss.useless_io_map = sizeof(tss) - 1;
	gpt[5] = gdt_entry_init((uint32_t) &tss, tss.useless_io_map, GDT_TSS);
	gpt[6] = (uint64_t) (uintptr_t) &tss >> 32;

	asm volatile ("mov %%rsp, %0" : "=r"(tss.rsp0) ::);

	gptr.base = (uint64_t)&gpt[0];
	gptr.limit = (uint16_t)sizeof(gpt) - 1;

	gdt_load(&gptr);

	return true;
}
