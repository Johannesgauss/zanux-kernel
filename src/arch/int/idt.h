#ifndef ARCH_AMD64_IDT_H
#define ARCH_AMD64_IDT_H

#include <stdint.h>
#include <stdbool.h>

#define IDT_MAX_GATES 256

struct gp_regs {
	void *rax;
	void *rbx;
	void *rcx;
	void *rdx;
	void *rsi;
	void *rdi;
	void *rbp;
	void *r8;
	void *r9;
	void *r10;
	void *r11;
	void *r12;
	void *r13;
	void *r14;
	void *r15;
};

struct hw_frame {
	void *rip;
	void *cs;
	uint64_t rflags;
	void *rsp;
	void *ss;
};

struct int_frame
{
	struct gp_regs gp;
	struct hw_frame hw;
};

struct idt_stack_call {
	struct gp_regs regs;
	uint64_t origin;
	uint64_t err_code;
	struct hw_frame hw;
};

static inline bool is_interrupt(struct idt_stack_call *call)
{
	return (int64_t)call->origin < 0;
}

static inline uint64_t get_vector(struct idt_stack_call *call)
{
	return ~call->origin;
}

static inline uint64_t get_syscall_nr(struct idt_stack_call *call)
{
	return call->origin;
}

bool idt_init();
void irqr_add(int irqr, void (*handler)(void));

#endif // ARCH_AMD64_IDT_H
