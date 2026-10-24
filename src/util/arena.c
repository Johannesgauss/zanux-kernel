// NOTE: arena.c requires standard library functions (calloc, free, printf)
// which are not available in the freestanding kernel environment.
// Disabled until a kernel memory allocator is implemented.
#if 0
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define PANICF() \
	printf("Double free!"); \
	exit(-1);

typedef struct Arena {
	void *mptr;
	size_t blk_size;
	size_t num; 
} Arena;

Arena arena_init(size_t blk_size, size_t num)
{
	Arena self;
	self.blk_size = blk_size;
	self.num = num;
	self.mptr = calloc(num, blk_size);

	return self;
}

void arena_free(Arena self)
{
	if (self.blk_size && self.num) 
		free(self.mptr);
	else
		PANICF()
} 
#endif

