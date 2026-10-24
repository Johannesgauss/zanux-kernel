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
	print_line("Welcome to Zanux Operating System\nCopyright (c). All rights reserved.\n");
	if (gdt_init())
		print_line("[ SUCCESS ] Global Descriptor Table (GDT) loaded. \n");
	
	if (timer_add())
		print_line("[ SUCCESS ] Programmable Interval Timer (PIT) setted on Interrupt Request 0 (IRQ0).\n");

	if (pmm_init())
		print_line("[ SUCCESS ] Physical Memory Managment (PMM) loaded.\n");

	if (vmm_init())
		print_line("[ SUCCESS ] Virtual Memory Managment (VMM) loaded.\n");

	proc_tmp_setup();

	syscall_init();
	if (idt_init())
		print_line("[ SUCCESS ] Interrupt Descriptor Table (IDT) loaded.\n");

	irqr_add(1, kb_handler);

	char *full_word = kmalloc(30);
	full_word = "Hello, World!\n";
	print_line(full_word);



	//asm volatile ("int $8");
	
	while (1) {
		asm volatile ("hlt");
	}
}
