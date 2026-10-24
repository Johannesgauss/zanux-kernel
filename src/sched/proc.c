#include "../mem/kmalloc.h"
#include "../util/alias.h"
#include "proc.h"
#include "test/test.h"
#include "proc_util.h"
#include "../drivers/gfx/print_text.h"

static struct proc proc_inst_inline(state_t state, virt_t ptr, uint stack_size);
static struct proc proc_create_inline(virt_t ptr, uint code_size, uint stack_size);
static void proc_kill_inline(struct proc *proc);
static void proc_exit_inline(int retvalue);
static struct proc *proc_sched_inline(struct int_frame *hf);
static void proc_suspend_inline(ushort id);
static void proc_switch_inline(struct int_frame *intf, ushort id_next);

void proc_kill(ushort id) { struct proc *proc = hash_table_id_search_inline(id); return proc_kill_inline(proc); }
void proc_exit(int retvalue) { return proc_exit_inline(retvalue); }
void proc_suspend(ushort id) { return proc_suspend_inline(id); }
void proc_switch(struct int_frame *intf, ushort id_next) { return proc_switch_inline(intf, id_next); }
struct proc proc_inst(state_t state, virt_t ptr, size_t stack_size) { return proc_inst_inline(state, ptr, stack_size); }

void proc_tmp_setup()
{
	current_proc = proc_create(&foo, 5000, 5000);
	proc_create(&fizz, 5000, 5000);
}

static inline struct proc proc_inst_inline(state_t state, virt_t ptr, uint stack_size)
{
	struct proc proc;

	proc.id = id_count;
	id_count++;

	proc.state = state;

	proc.ctx.intf.gp = (struct gp_regs) {0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	return proc;
}


static inline void proc_kill_inline(struct proc *proc)
{
	proc->state = PROC_DEAD;
	vfree(proc->stbase);
}

static inline void proc_exit_inline(int retvalue)
{
	current_proc->retvalue = retvalue;
	proc_kill_inline(current_proc);
}

static inline void proc_suspend_inline(ushort id)
{
	struct proc *proc = hash_table_id_search_inline(id);
	proc->state = PROC_READY;
}

static inline void proc_switch_inline(struct int_frame *intf, ushort id_next)
{
	if (in_user_mode(intf)) {
		current_proc->ctx.intf = *intf;
	}

	current_proc->state = PROC_READY;

	current_proc = hash_table_id_search_inline(id_next);
	current_proc->state = PROC_EXEC;
}

struct proc *proc_create(virt_t ptr, size_t code_size, size_t stack_size)
{

	struct proc *proc = kmalloc(sizeof(*proc));
	*proc = proc_inst_inline(PROC_READY, ptr, stack_size);

	arch_proc_init(&proc->ctx);

	proc->ctx.intf.hw.rsp = usr_kmalloc(stack_size) + stack_size;
	proc->stbase = proc->ctx.intf.hw.rsp;
	proc->ctx.intf.hw.rflags = (1 << 9) | 0b10;
	// Considering real programs, they will be loaded from storage, so this
	// is just a dummy implementation anyway, so
	// TODO! substitute this.

	proc->ctx.intf.hw.rip = usr_vmalloc(code_size);
	memcpy(proc->ctx.intf.hw.rip, ptr, code_size);

	// Add it into hash hash table
	hash_table_id_insert(proc);

	return proc;
}

struct proc *proc_sched(struct int_frame *intf)
{
	// TODO: I don't think this is the better solution!
	if (in_user_mode(intf)) {
		current_proc->ctx.intf = *intf;
		//print_hex((uint64_t)current_proc->ctx.intf.rip);
		//print_hex((uint64_t)current_proc->ctx.intf.rsp);
	}

	if (current_proc->id == 0 && hash_table_id_search_inline(1)->state != PROC_DEAD) {
		proc_switch_inline(intf, 1);
	} else if (current_proc->id == 1 && hash_table_id_search_inline(0)->state != PROC_DEAD){
		proc_switch_inline(intf, 0);		
	} else {
		while (1) {
			asm volatile ("hlt");
		}
	}

	return current_proc;
}
