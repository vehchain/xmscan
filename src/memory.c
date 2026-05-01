#include "memory.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_memory_range(char *path, char *lib, uintptr_t *low, uintptr_t *high) {
	if (!path || !lib || !low || !high) {
		return 0;
	}

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		fprintf(stderr, "failed to open file \"%s\" (%s)\n", path, strerror(errno));
		return 1;
	}

	*low = 0;
	*high = 0;
	char line[4224];
	while (fgets(line, 4224, file)) {
		if (strstr(line, lib)) {
			char *separator = NULL;
			uintptr_t base, end;
			base = strtoull(line, &separator, 16);
			end = strtoull(&separator[1], NULL, 16);

			if (*low == 0) {
				*low = base;
			}

			if (*high < end) {
				*high = end;
			}
		}
	}

	return *low && *high;
}
