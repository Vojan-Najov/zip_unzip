#ifndef ZIP_UNZIP_CHUNK_H_
#define ZIP_UNZIP_CHUNK_H_

#include <stddef.h>

typedef struct chunk {
	char *memptr;
	size_t size;
	size_t capacity;
} chunk_t;

int allocate_chunk(chunk_t *chunk, size_t size);

void free_chunk(chunk_t *chunk);

#endif  /* ZIP_UNZIP_CHUNK_H_ */
