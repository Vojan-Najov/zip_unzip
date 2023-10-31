#ifndef ZIP_UNZIP_MAPPER_H_
#define ZIP_UNZIP_MAPPER_H_

#include <stddef.h>

#include "chunk.h"

#define MAPPER_NO_ERROR 0
#define MAPPER_STATFILE_ERROR 1
#define MAPPER_REGFILE_ERROR 2
#define MAPPER_OPENFILE_ERROR 3
#define MAPPER_CLOSEFILE_ERROR 4
#define MAPPER_MAP_ERROR 5
#define MAPPER_EMPTYFILE_ERROR 6

typedef struct mapper {
	int fildes;
	size_t filesize;
	size_t chunksize;
	size_t offset;
} mapper_t;

int init_mapper(mapper_t *mapper, const char *pathname);

int close_mapper(mapper_t *mapper);

int next_chunk(mapper_t *mapper);

int pop_chunk(mapper_t *mapper, chunk_t *chunk);

int put_chunk(chunk_t * chunk);

#endif  /* ZIP_UNZIP_MAPPER_H_ */
