#include "util/alias.h"
#include "arch/sys/syscall.h"
#include "arch/gdt_macros.h"

void sys_write(int fd, char *str, umax str_size);
typedef int64_t (*sys_call_t)(umax,umax,umax,umax,umax,umax);

#define SYSCALL_NUMBER 2
const sys_call_t syscall_table[SYSCALL_NUMBER] = {
	(sys_call_t)sys_exit,
	(sys_call_t)sys_write,
};
