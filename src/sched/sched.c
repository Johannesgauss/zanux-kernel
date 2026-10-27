#include "sched/proc.h"
#include "proc_util.h"

// struct proc *current_proc = the current process
struct proc *proc_sched()
{
	if (!current_proc) return hash_table_id_search(0);
	struct proc *p0 = hash_table_id_search(0);
	struct proc *p1 = hash_table_id_search(1);
	if (current_proc->id == 0 && p1 && p1->state != PROC_DEAD) return p1;
	if (current_proc->id == 1 && p0 && p0->state != PROC_DEAD) return p0;
	if (current_proc->state != PROC_DEAD) return current_proc;
	return NULL;
}
