#ifndef ZIP_UNZIP_MAPPER_H_
#define ZIP_UNZIP_MAPPER_H_

#define NEED_ALLIGN 1
#define DEFAULT_CHUNKSIZE 4096

#define MAPPER_NO_ERROR 0
#define MAPPER_STATFILE_ERROR 1
#define MAPPER_REGFILE_ERROR 2
#define MAPPER_OPENFILE_ERROR 3
#define MAPPER_CLOSEFILE_ERROR 4
#define MAPPER_MAP_ERROR 5

#include <stddef.h>

typedef struct chunk {
	char *memptr;
	size_t size;
} chunk_t;

typedef struct mapper {
	int fildes;
	size_t filesize;
	size_t chunksize;
	size_t offset;
} mapper_t;

int init_mapper(mapper_t *mapper, const char *pathname, size_t chunksize);

int close_mapper(mapper_t *mapper);

int next_chunk(mapper_t *mapper);

int pop_chunk(mapper_t *mapper, chunk_t *chunk);

int put_chunk(chunk_t * chunk);

#endif  /* ZIP_UNZIP_MAPPER_H_ */
