#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "mapper.h"

#ifndef MAPPING_FILE

int init_mapper(mapper_t &mapper, const char *pathname, size_t chunksize)
{
	struct stat stat_buffer;
	int err_status = MAPPER_NO_ERROR;

	err_status = stat(pathname, &stat_buffer);
	if (err_status) {
		return MAPPER_STATFILE_ERROR;
	}
	if (stat_buffer.st_mode & S_IFMT != S_IFREG) {
		return MAPPER_REGFILE_ERROR;
	}
	mapper->filesize = stat_buffer.st_size;

	if (!chunksize) {
		chunksize = DEFAULT_CHUNKSIZE;
	}
	mapper->chunksize = chunksize;

	mapper->stream = fopen(pathname, "r");
	if (NULL == mapper->stream) {
		return MAPPER_OPENFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

int close_mapper(mapper_t *mapper)
{
	if (fclose(mapper->stream)) {
		return MAPPER_CLOSEFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

int next_chunk(mapper_t *mapper)
{
	return mapper->filesize > mapper->offset;
}

int pop_chunk(mapper_t *mapper, chunk_t *chunk)
{
	size
}

#else /* MAPPING FILE */

int init_mapper(mapper_t *mapper, const char *pathname, size_t chunksize)
{
	size_t pagesize;
	struct stat stat_buffer;
	int err_status = MAPPER_NO_ERROR;

	err_status = stat(pathname, &stat_buffer);
	if (err_status) {
		return MAPPER_STATFILE_ERROR;
	}
	if (stat_buffer.st_mode & S_IFMT != S_IFREG) {
		return MAPPER_REGFILE_ERROR;
	}
	mapper->filesize = stat_buffer.st_size;

	pagesize = getpagesize();
	if (!chunksize) {
		chunksize = DEFAULT_CHUNKSIZE;
	}
	chunksize = (chunksize / pagesize + 1) * pagesize;
	mapper->chunksize = chunksize;

	mapper->offset = 0;

	mapper->fildes = open(pathname, O_RDONLY);
	if (mapper->fildes == -1) {
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

	chunk->memptr = (char *) mmap(NULL, len, PROT_READ, MAP_SHARED, \
                                mapper->fildes, mapper->offset);
	if (chunk->memptr == MAP_FAILED) {
		perror("map error");
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

	if (chunk->memptr != MAP_FAILED) {
		munmap(chunk->memptr, chunk->size);
	} else {
		err_status = MAPPER_MAP_ERROR;
	}

	return err_status;
}

#endif /* MAPPING_FILE */
