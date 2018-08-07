#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "vm/vm.h"
#include "utils/gmalloc.h"
#include "utils/module.h"

struct modules {
	unsigned nmod;
	int fds[];
} *mod_list;

int init_modules(unsigned nmod, char *list[]){
	if(!(mod_list = (struct modules*)malloc(sizeof(struct modules)+sizeof(int)*nmod)))
		return -1;

	mod_list->nmod = nmod;
	for(int i = 0; i < nmod; i++)
		if((mod_list->fds[i] = open(list[i], O_RDONLY)) < 0)
			perror(list[i]);

	return 0;
}

int load_module(struct vm *vm, int id, uint64_t addr, size_t size){
	int nmod;
	int fd;

	if(!mod_list)
		return -1;
	nmod = mod_list->nmod;

	if(id >= nmod)
		return -1;
	fd = mod_list->fds[id];

	if(fd < 0)
		return -1;

	if(!size){
		struct stat stbuf;

		if(fstat(fd, &stbuf) == -1){
			perror("fstat");
			return -1;
		}

		size = stbuf.st_size;
	}

	if((addr = gmalloc(addr, (size + 0x1000-1) & ~0xfff)) == -1){
		perror("gmalloc");
		return -1;
	}

	read(fd, guest2phys(vm, addr), size);
	close(fd);
	mod_list->fds[id] = -1;

	return addr;
}

int load_kernel(struct vm *vm){
	return load_module(vm, 0, 0, 0);
}
