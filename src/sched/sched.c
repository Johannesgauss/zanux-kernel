/*#include "proc.h"

void proc_tmp_setup()
{
	procs[0] = proc_create(&foo, 5000, 5000);
	procs[1] = proc_create(&fizz, 5000, 5000);
}

struct proc *proc_sched(struct int_frame *intf)
{
	// TODO: I don't think this is the better solution!
	static bool defined = false;
	if (defined) {
		procs[current_proc_id].ctx.intf = *intf;
		//print_hex((uint64_t)procs[current_proc_id].ctx.intf.rip);
		//print_hex((uint64_t)procs[current_proc_id].ctx.intf.rsp);
	} else defined = true;
	if (current_proc_id == 0) {
		print_line("fizz\n");
		current_proc_id = 1;
	} else {
		print_line("foo!\n");
		current_proc_id = 0;
	}
	return &procs[current_proc_id];
} */
