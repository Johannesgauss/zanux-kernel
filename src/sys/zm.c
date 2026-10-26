#include "util/alias.h"
#include <stdarg.h>

struct zm_t;
typedef void fn_t;
typedef int64_t (*zm_fn_t)();


// Zanux Machine
#define ZM_CALL1(zm, a1) \
(((int64_t (*)(struct zm_t *, umax))(zm)->fn)((zm), (umax)(a1)))                                                                          

#define ZM_CALL2(zm, a1, a2) \
(((int64_t (*)(struct zm_t *, umax, umax))(zm)->fn)((zm), (umax)(a1), (umax)(a2)))                                                        
																	  
#define ZM_CALL3(zm, a1, a2, a3) \
(((int64_t (*)(struct zm_t *, umax, umax, umax))(zm)->fn)((zm), (umax)(a1), (umax)(a2), (umax)(a3)))

struct zm_t {
	virt_t tape;
	umax tape_size;
	zm_fn_t fn;
};

void a()
{
	struct zm_t a;
	ZM_CALL1(&a, 3);
}
