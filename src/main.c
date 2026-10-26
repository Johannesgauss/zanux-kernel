#include "drivers/gfx/print_text.h"
#include "arch/gdt.h"
#include "arch/int/idt.h"
#include "drivers/clk/clk.h"

extern void kb_handler(void);
extern void vmm_map(void *addr);
extern void print_hex(uint64_t val);
extern bool pmm_init();
extern bool vmm_init();
extern void vmm_free(void *addr);
extern void *kmalloc(size_t size);
extern void proc_tmp_setup();
extern void syscall_init();

void kmain() 
{
	gfx_init();
	kprint("Welcome to Zanux Operating System\nCopyright (c). All rights reserved.\n");
	if (gdt_init())
		kprint("[ SUCCESS ] Global Descriptor Table (GDT) loaded. \n");
	
	if (timer_add())
		kprint("[ SUCCESS ] Programmable Interval Timer (PIT) setted on Interrupt Request 0 (IRQ0).\n");

	if (pmm_init())
		kprint("[ SUCCESS ] Physical Memory Managment (PMM) loaded.\n");

	if (vmm_init())
		kprint("[ SUCCESS ] Virtual Memory Managment (VMM) loaded.\n");

	proc_tmp_setup();

	syscall_init();
	if (idt_init())
		kprint("[ SUCCESS ] Interrupt Descriptor Table (IDT) loaded.\n");

	irqr_add(1, kb_handler);

	char *full_word = kmalloc(30);
	full_word = "Hello, World!\n";
	kprint(full_word);



	//asm volatile ("int $8");
	
	while (1) {
		asm volatile ("hlt");
	}
}
