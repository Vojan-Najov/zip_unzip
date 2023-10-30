#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "zlib.h"
#include "mapper.h"

static int write_out_fildes(const Bytef *buf, size_t nbytes, int fildes)
{
	ssize_t nwrite;
	ssize_t offset;

	offset = 0;
	do {
		nwrite = write(fildes, buf, nbytes);
		if (nwrite == -1) {
			fprintf(stderr, "%s: %s: %s\n", "zip", "output file", "write failed");
			return EXIT_FAILURE;
		} else {
			offset += nwrite;
			nbytes -= nwrite;
		}
	} while (nbytes > 0);

	return EXIT_SUCCESS;
}

int unzip_decompress(mapper_t *mapper, int out_fildes)
{
	int zstatus;
	int err_status;
	size_t ndecompressed;
	chunk_t chunk;
	Bytef *out_buffer;
	z_stream stream = {0};

	err_status = inflateInit(&stream);
	if (err_status != Z_OK) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "zlib", stream.msg);
		return EXIT_FAILURE;
	}

	fprintf(stderr, "1\n");

	out_buffer = (Bytef *) malloc(sizeof(Bytef) * mapper->chunksize);
	if (out_buffer == NULL) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "memory", strerror(errno));
		err_status = EXIT_FAILURE;
	}

	fprintf(stderr, "2\n");

	while (next_chunk(mapper) && !err_status) {
		err_status = pop_chunk(mapper, &chunk);
		if (err_status) {
			break;
		}

		stream.avail_in = chunk.size;
		stream.next_in = chunk.ptr;

		do {
			stream.avail_out = mapper->chunksize;
			stream.next_out = out_buffer;

			zstatus = inflate(&stream, Z_NO_FLUSH);
			if (zstatus != Z_OK && zstatus != Z_STREAM_END) {
				fprintf(stderr, "1%s: %s: %s\n", "unzip", "zlib", stream.msg);
				err_status = EXIT_FAILURE;
				break;
			}

			ndecompressed = mapper->chunksize - stream.avail_out;
			
			err_status = write_out_fildes(out_buffer, ndecompressed, out_fildes);

		} while (!stream.avail_out && !err_status);

		put_chunk(&chunk);
	}

	if (inflateEnd(&stream) != Z_OK) {
		err_status = EXIT_FAILURE;
		fprintf(stderr, "%s: %s: %s\n", "zip", "zlib", stream.msg);
	}

	free(out_buffer);

	return err_status;
}
