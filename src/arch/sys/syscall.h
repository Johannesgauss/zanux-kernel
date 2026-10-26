#ifndef ARCH_SYS_SYCALL_H
#define ARCH_SYS_SYCALL_H

#include "util/alias.h"
#include <stddef.h>
#define EFER_ID 0xC0000080
#define STAR_ID 0xC0000081
#define LSTAR_ID 0xC0000082
#define CSTAR_ID 0xC0000083
#define SF_MASK_ID 0xC0000084

inline void wrmsr(uint32_t msr_id, uint64_t value)
{
	asm volatile ("wrmsr":: "a"((uint32_t)(value & 0xFFFFFFFF)), "d"((uint32_t)(value >> 0x20)), "c"(msr_id));
}

inline uint64_t rdmsr(uint32_t msr_id)
{
	uint32_t high, low;	

	asm volatile ("rdmsr": "=a"(low), "=d"(high) : "c"(msr_id));

	return ((uint64_t)high << 0x20) + low;
}
extern void sys_exit(int retvalue);

#endif // ARCH_SYS_SYCALL_H
