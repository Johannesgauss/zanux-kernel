#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "drivers/gfx/print_text.h"
#include "drivers/port.h"
#include "idt.h"
#include "irq.h"
#include "drivers/clk/clk_macros.h"
#include "arch/gdt_macros.h"

//  P DPL paper type
//0b1 00   0    1110
#define IDT_FLAGS 0b10001110
#define IDT_SYSCALL_FLAGS 0b11101110

extern void *irq_routines[16];
struct idt_gate {
	uint16_t offset_low;
	uint16_t segment;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved;
} __attribute__((packed));

struct idtr_t {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct idt_gate idt[IDT_MAX_GATES];
struct idtr_t idtr;

extern void *idt_stub_table[IDT_MAX_GATES];
extern void idt_load(struct idtr_t *idtr);

static struct idt_gate idt_set_gate(uint64_t offset, uint16_t segment, uint8_t flags, uint8_t ist)
{
	struct idt_gate gate;

	gate.offset_low = (uint16_t) offset; 
	gate.offset_mid = (uint16_t) (offset >> 0x10); 
	gate.offset_high = (uint32_t) (offset >> 0x20); 

	gate.segment = segment;
	gate.flags = flags;
	gate.ist = ist; // TODO it MUST have the possibility of being 1
	
	return gate;
}

void irq_remap()
{
	// TODO: hardcoded values!
	outportb(PIC1_COMMAND, 0x11); io_wait();
	outportb(PIC2_COMMAND, 0x11); io_wait();

	outportb(PIC1_DATA, 0x20); io_wait();
	outportb(PIC2_DATA, 0x28); io_wait();

	outportb(PIC1_DATA, 0x04); io_wait();
	outportb(PIC2_DATA, 0x02); io_wait();

	outportb(PIC1_DATA, 0x01); io_wait();
	outportb(PIC2_DATA, 0x01); io_wait();

	outportb(PIC1_DATA, 0x0);
	outportb(PIC2_DATA, 0x0);
}

extern void *int_handler();
extern void syscall_handler(int code, int param);
bool idt_init()
{
	irq_remap();

	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(struct idt_gate) * IDT_MAX_GATES - 1;

	for (int i = 0; i < IDT_MAX_GATES; i++) {
		idt[i] = idt_set_gate((uint64_t)idt_stub_table[i], GDT_KERNEL_CODE, IDT_FLAGS, 0);
	}
	idt[32] = idt_set_gate((uint64_t)int_handler, GDT_KERNEL_CODE, IDT_FLAGS, 0);
	idt[0x80] = idt_set_gate((uint64_t)syscall_handler, GDT_KERNEL_CODE, IDT_SYSCALL_FLAGS, 0);
;

	asm volatile ( "lidt %0" : : "m"(idtr));
	asm volatile ( "sti" );

	return true;
}

void exception_handler(struct idt_stack_call *regs)
{
	kprint("\n--- KERNEL PANIC: CPU EXCEPTION ---\n");
	kprint("Exception: ");

	uint64_t int_no = get_vector(regs);
	switch (int_no) {
	case 0:  kprint("0 - Division by Zero (#DE)\n"); break;
	case 1:  kprint("1 - Debug (#DB)\n"); break;
	case 2:  kprint("2 - Non-Maskable Interrupt (#NMI)\n"); break;
	case 3:  kprint("3 - Breakpoint (#BP)\n"); break;
	case 4:  kprint("4 - Overflow (#OF)\n"); break;
	case 5:  kprint("5 - Bound Range Exceeded (#BR)\n"); break;
	case 6:  kprint("6 - Invalid Opcode (#UD)\n"); break;
	case 7:  kprint("7 - Device Not Available (#NM)\n"); break;
	case 8:  kprint("8 - Double Fault (#DF)\n"); break;
	case 9:  kprint("9 - Coprocessor Segment Overrun\n"); break;
	case 10: kprint("10 - Invalid TSS (#TS)\n"); break;
	case 11: kprint("11 - Segment Not Present (#NP)\n"); break;
	case 12: kprint("12 - Stack-Segment Fault (#SS)\n"); break;
	case 13: kprint("13 - General Protection Fault (#GP)\n"); break;
	case 14: kprint("14 - Page Fault (#PF)\n"); break;
	case 15: kprint("15 - Reserved\n"); break;
	case 16: kprint("16 - x87 Floating-Point Exception (#MF)\n"); break;
	case 17: kprint("17 - Alignment Check (#AC)\n"); break;
	case 18: kprint("18 - Machine Check (#MC)\n"); break;
	case 19: kprint("19 - SIMD Floating-Point Exception (#XM)\n"); break;
	case 20: kprint("20 - Virtualization Exception (#VE)\n"); break;
	case 21: kprint("21 - Control Protection Exception (#CP)\n"); break;
	case 28: kprint("28 - Hypervisor Injection Exception (#HV)\n"); break;
	case 29: kprint("29 - VMM Communication Exception (#VC)\n"); break;
	case 30: kprint("30 - Security Exception (#SX)\n"); break;
	default: kprint("Reserved / Unknown Exception\n"); break;
	}

	kprint("RIP:    "); print_hex((uint64_t)regs->hw.rip);      kprint("\n");
	kprint("CS:     "); print_hex((uint64_t)regs->hw.cs);       kprint("\n");
	kprint("RFLAGS: "); print_hex((uint64_t)regs->hw.rflags);   kprint("\n");
	kprint("RSP:    "); print_hex((uint64_t)regs->hw.rsp);      kprint("\n");
	kprint("ERR:    "); print_hex((uint64_t)regs->err_code);    kprint("\n");

	while (1) {
		asm volatile ("cli; hlt");
	}
}

void proc_tmp_setup();
void idt_handler(struct idt_stack_call *regs)
{
	uint64_t int_no = get_vector(regs);
	if (int_no < 32) {
		exception_handler(regs);
	} else {
		void (*handler)(void) = irq_routines[int_no - 32];
		//if (int_no == 32) { int_handler(); }
		if (handler)
			handler();
		if (int_no >= 40) {
			outb(PIC2_COMMAND, PIC_EOI);
		}
		outb(PIC1_COMMAND, PIC_EOI);
	}
}
