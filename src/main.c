#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "memory.h"
#include "pattern.h"

int main(int argc, char **argv) {
	char path[32];
	pid_t pid;
	void *memory_block;
	uintptr_t base, end;
	size_t size;

	if (argc < 2) {
		fprintf(stderr, "%s pid lib_name pattern\n", argv[0]);
		return 1;
	}

	pid = strtoul(argv[1], NULL, 0);
	if (pid == 0) {
		fprintf(stderr, "invalid pid \n");
		return 1;
	}

	sprintf(path, "/proc/%u/maps", pid);

	if (!get_memory_range(path, argv[2], &base, &end)) {
		fprintf(stderr, "get_memory_range failed (%s)\n", strerror(errno));
		return 1;
	}

	size = end - base;
	memory_block = malloc(size);
	if (!memory_block) {
		fprintf(stderr, "memory_block failed (%s)\n", strerror(errno));
		return 1;
	}

	struct iovec dst;
	dst.iov_base = memory_block;
	dst.iov_len = size;

	struct iovec src;
	src.iov_base = (void *) base;
	src.iov_len = size;

	if (process_vm_readv(pid, &dst, 1, &src, 1, 0) != size) {
		fprintf(stderr, "process_vm_readv failed (%s)\n", strerror(errno));
		return 1;
	}

	off_t offset = pattern_scan(memory_block, size, argv[3]);
	free(memory_block);

	if (offset != 0) {
		printf("pattern found in %p at offset %lu\n", (uint8_t *) base + offset, offset);
	} else {
		fprintf(stderr, "pattern not found\n");
		return 1;
	}

	return 0;
}
