#include "util/alias.h"
#include "arch/sys/syscall.h"
#include "arch/gdt_macros.h"
#include "drivers/gfx/print_text.h"

void sys_write(int fd, char *str, umax str_size)
{
	kprint(str);
}
