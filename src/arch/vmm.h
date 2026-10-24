#ifndef ARCH_AMD64_VMM
#define ARCH_AMD64_VMM
#include <stdint.h>
#include <stdbool.h>

#define SET_P    1ULL
#define SET_RW  (1ULL << 1)
#define SET_US  (1ULL << 2)
#define SET_PWT (1ULL << 3)
#define SET_PCD   (1ULL << 4)
#define SET_A (1ULL << 5)
#define SET_IGN (1ULL << 6)
#define SET_MBZ (0 << 7) | (0 << 8)
//#define SET_ADDR(entry) entry << 12
#define SET_NX  (1ULL << 63)
#define GET_PHY_ADDR (0x000FFFFFFFFFF000ULL)


//static uint64_t *pml4;
void vmm_map(void *addr);

bool vmm_init();

// TODO a better vmm_free
void vmm_free(void *addr);
extern uint64_t *g_pml4;


#endif // ARCH_AMD64_VMM
