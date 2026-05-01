#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

int get_memory_range(char *path, char *lib, uintptr_t *low, uintptr_t *high);

#endif
