#define _GNU_SOURCE

#include "pattern.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pattern_byte {
	uint8_t byte;
	bool is_wildcard[2];
};

struct token_vector {
	struct pattern_byte *vec;
	size_t capacity;
	size_t used;
};

static bool init_vec(struct token_vector *vector) {
	vector->vec = calloc(128, sizeof(struct pattern_byte));
	if (vector->vec == NULL) {
		return false;
	}

	vector->capacity = 128;
	vector->used = 0;
	return true;
}

static bool grow_vec(struct token_vector *vector) {
	struct pattern_byte *tmp = realloc(vector->vec, vector->capacity + 128);
	if (!tmp) {
		return false;
	}

	vector->capacity += 128;
	vector->vec = tmp;
	return true;
}

static bool push_token(char *token, struct token_vector *vector) {
	if (!token || !vector) {
		fprintf(stderr, "invalid parameters\n");
		return false;
	}

	if (vector->vec == NULL) {
		fprintf(stderr, "unitialized vector\n");
		return false;
	}

	if (vector->capacity == vector->used) {
		if (!grow_vec(vector)) {
			fprintf(stderr, "grow_vec failed (%s)\n", strerror(errno));
			return false;
		}
	}

	if (isspace(token[1]) || token[1] == '\0') {
		if (!isxdigit(token[0])) {
			if (token[0] != '?') return false;

			vector->vec[vector->used].is_wildcard[0] = true;
			vector->vec[vector->used].is_wildcard[1] = true;
		} else {
			char low = tolower(token[0]);
			if (low >= 'a' && low <= 'f')
				vector->vec[vector->used].byte = (low - 'a') + 10;
			else
				vector->vec[vector->used].byte = low - '0';
		}
	} else {
		for (int i = 0; i < 2; i++) {
			if (!isxdigit(token[i])) {
				if (token[i] != '?') return false;

				vector->vec[vector->used].is_wildcard[i] = true;
			} else {
				char low = tolower(token[i]);
				if (low >= 'a' && low <= 'f')
					vector->vec[vector->used].byte |= ((low - 'a') + 10) << (i ? 0 : 4);
				else
					vector->vec[vector->used].byte |= (low - '0') << (i ? 0 : 4);
			}
		}
	}

	vector->used++;
	return true;
}

off_t pattern_scan(char *base, size_t size, char *pattern) {
	struct token_vector list;
	if (!init_vec(&list)) {
		fprintf(stderr, "init_vec failed (%s)\n", strerror(errno));
		return 1;
	}

	for (int i = 0; i < strlen(pattern); i++) {
		if (isspace(pattern[i])) {
			continue;
		}
		if (!push_token(&pattern[i], &list)) {
			fprintf(stderr, "invalid token\n");
			free(list.vec);
			return 0;
		}

		i++;
	}

	if (list.used == 0) {
		fprintf(stderr, "no bytes identified in pattern\n");
		free(list.vec);
		return 0;
	}

	for (size_t i = 0; i < size; i++) {
		bool found = 1;

		for (size_t j = 0; j < list.used; j++) {
			uint8_t current = base[i + j];
			if (list.vec[j].is_wildcard[0]) {
				if (list.vec[j].is_wildcard[1]) {
					continue;
				}

				if ((current & 0b1111) != (list.vec[j].byte & 0b1111)) {
					found = 0;
					break;
				}
			} else if (list.vec[j].is_wildcard[1]) {
				if ((current & 0b11110000) != (list.vec[j].byte & 0b11110000)) {
					found = 0;
					break;
				}
			} else if (current != list.vec[j].byte) {
				found = 0;
				break;
			}
		}

		if (found) {
			free(list.vec);
			return i;
		}
	}

	free(list.vec);
	return 0;
}
