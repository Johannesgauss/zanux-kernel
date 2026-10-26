#ifndef SCHED_PROC
#define SCHED_PROC

#include "util/alias.h"
#include "arch/int/idt.h"
#include "arch/int/arch_proc.h"
#include <stddef.h>

typedef enum {
	PROC_DEAD = 0,
	PROC_EXEC = 1,
	PROC_BLOCK = 2,
	PROC_READY = 3
} state_t;

struct proc {
	struct proc *next;
	struct proc *prev;
	struct arch_proc ctx;
	virt_t stbase;
	state_t state;
	utiny id;
	utiny retvalue;
};

struct proc proc_inst(state_t state, virt_t ptr, size_t stack_size);
struct proc *proc_create(virt_t ptr, size_t code_size, size_t stack_size);

void proc_kill(ushort id);
void proc_exit(int retvalue);
struct proc *proc_int(struct int_frame *hf);

#endif // SCHED_PROC
