#include "mem/pmm.h"
#include "mem/kmalloc.h"
#include "util/alias.h"
#include "proc.h"
#include "test/test.h"
#include "util/memory.h"

static struct proc *hash_table_id_search(uint id);
static void hash_table_id_insert(struct proc *proc);

// TODO: a better implementation
#define HASH_TABLE_LOG 8
#define PROC_HASH_TABLE_SIZE (1 << HASH_TABLE_LOG)
static struct proc *proc_hash_table[PROC_HASH_TABLE_SIZE];

static ushort id_count = 0;
static struct proc *current_proc = NULL;

#define GOLDEN_RATIO_32_SCALED 0x9E3779B9U
static inline uint hash_table_id_gen(uint id)
{
	return (id * GOLDEN_RATIO_32_SCALED) >> (32 - HASH_TABLE_LOG);
}

static struct proc *hash_table_id_search_inline(uint id)
{
	uint htid = hash_table_id_gen(id);
	struct proc *current = proc_hash_table[htid];
	while (current && current->id != id) {
		current = current->next;	
	}

	return current;
}
static struct proc *hash_table_id_search(uint id) { return hash_table_id_search_inline(id); }

static void hash_table_id_insert(struct proc *proc)
{
	uint ht_id = hash_table_id_gen(proc->id);
	struct proc *bottom = proc_hash_table[ht_id];
	if (bottom) {
		bottom->prev = proc;
		proc->next = bottom;
	} else {
		proc->next = NULL;
	}
	proc->prev = NULL;
	proc_hash_table[ht_id] = proc;
}

static void hash_table_id_rm(uint id)
{
	struct proc *current = hash_table_id_search_inline(id);	

	if (current->prev)
		current->prev->next = current->next;
	else
		proc_hash_table[id] = current->next;

	if (current->next)
		current->next->prev = current->prev;

	kfree(current);
}
