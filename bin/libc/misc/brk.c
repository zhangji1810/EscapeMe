#include <stdint.h>
#include <stddef.h>
#include "../syscall.h"

static void *__curbrk;

int brk(void *addr){
	void *res;

	syscall(res, NR_brk);

	if(res < 0)
		return -1;

	__curbrk = (void*)res;
	return 0;
}

void *sbrk(intptr_t increment){
	void *oldbrk;

	if (__curbrk == NULL && brk(0) < 0)
		return (void *) -1;

	if (increment == 0)
		return __curbrk;

	oldbrk = __curbrk;
	if (increment > 0
			? ((uintptr_t) oldbrk + (uintptr_t) increment < (uintptr_t) oldbrk)
			: ((uintptr_t) oldbrk < (uintptr_t) -increment))
		return (void *) -1;

	if (brk(oldbrk + increment) < 0)
		return (void *) -1;

	return oldbrk;
}
