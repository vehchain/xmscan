#ifndef PATTERN_H
#define PATTERN_H

#include <stddef.h>
#include <sys/types.h>

off_t pattern_scan(char *base, size_t size, char *pattern);

#endif
