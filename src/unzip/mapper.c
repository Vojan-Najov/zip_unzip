#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mapper.h"

/* Количество страниц виртуальной памяти, отведенных под "маппирование" файла */
#define DEFAULT_NPAGE 4


int init_mapper(mapper_t *mapper, const char *pathname)
{
	struct stat stat_buffer;
	int err_status = MAPPER_NO_ERROR;

	err_status = stat(pathname, &stat_buffer);
	if (err_status) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, strerror(errno));
		return MAPPER_STATFILE_ERROR;
	}
	if ((stat_buffer.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, "not a regular file");
		return MAPPER_REGFILE_ERROR;
	}

	mapper->filesize = stat_buffer.st_size;

	mapper->chunksize = DEFAULT_NPAGE * getpagesize();
	
	mapper->offset = 0;

	mapper->fildes = open(pathname, O_RDONLY);
	if (mapper->fildes == -1) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, strerror(errno));
		return MAPPER_OPENFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

int close_mapper(mapper_t *mapper) {
	if (close(mapper->fildes)) {
		return MAPPER_CLOSEFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

int next_chunk(mapper_t *mapper)
{
	return mapper->filesize > mapper->offset;
}

int pop_chunk(mapper_t *mapper, chunk_t *chunk) {
	size_t len;
	int err_status = MAPPER_NO_ERROR;

	if (mapper->filesize - mapper->offset >= mapper->chunksize) {
		len = mapper->chunksize;
	} else {
		len = mapper->filesize - mapper->offset;
	}

	fprintf(stderr, "size %zu    offset %zu\n", len, mapper->offset);

	chunk->ptr = (unsigned char *) mmap(NULL, len, PROT_READ, MAP_SHARED, \
                                mapper->fildes, mapper->offset);
	if (chunk->ptr == MAP_FAILED) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "mapping", strerror(errno));
		err_status = MAPPER_MAP_ERROR;
		chunk->size = 0;
	} else {
		chunk->size = len;
		mapper->offset += len;
	}

	return err_status;
}

int put_chunk(chunk_t *chunk) {
	int err_status = MAPPER_NO_ERROR;

	if (chunk->ptr != MAP_FAILED) {
		munmap(chunk->ptr, chunk->size);
	} else {
		err_status = MAPPER_MAP_ERROR;
	}

	return err_status;
}

