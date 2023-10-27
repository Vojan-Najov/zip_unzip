
#include "params.h"
#include "mapper.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	chunk_t chunk;
	params_t params;
	mapper_t mapper;
	int err_status = 0;

	err_status = parse_params(argc, argv, &params);
	if (err_status) {
		fwrite(HELP_MESSAGE, sizeof(char), sizeof(HELP_MESSAGE), stderr);
		exit(EXIT_FAILURE);
	}
	//if (params.dst_pathname == NULL) {
		//generate_zip_pathname(&params.dst_pathname);
	//}
	
	err_status = init_mapper(&mapper, params.src_pathname, 0);
	if (err_status) {
		fprintf(stderr, "init_mapper_error\n");
		return 1;
	}

	while (next_chunk(&mapper)) {
		err_status = pop_chunk(&mapper, &chunk);
		if (err_status) {
			fprintf(stderr, "pop_chunk_error\n");
			return 1;
		}

		fwrite(chunk.memptr, sizeof(char), chunk.size, stdout);

		//err_status = put_chunk(&chunk);
		//if (err_status) {
		//	fprintf(stderr, "put_chunk_error\n");
		//	return 1;
		//}
	}

	return 0;
}

/*
int main(int argc, char **argv)
{
	mapper_t mapper;
	int err_status = 0;
	size_t chunk_size = 0;

	if (argc != 3 ) {
		return 1;
	}

	err_status = init_mapper(&mapper, argv[1], chunk_size);
	if (err_status) {
		perrror(argv[1]);
		exit(EXIT_FAILURE);
	}

	chunk_t chunk;

	while (next_chunk(&mapper)) {
		err_status = pop_chunk(&mapper, &chunk);
		if (err_status) {
			// handle mmap error
			break;
		}

		zip(&src_chunk, &dst_chunk)

		err_status = write(dst_fd, dst_chunk.memptr, dst_chunk.size);
		if (err_status) {
			//handle write error
			break;
		}

		err_status = put_chunk(&chunk);
		if (err_status) {
			// handle unmup error;
			break;
		}
	}

	err_status = close_mapper(&mapper);
	if (err_status) {
		perrror(argv[1]);
		exit(EXIT_FAILURE);
	}

	err_status = close(fd_dst);
	if (err_status) {
		//handle close dst error
	}

	return 0;
}

int zip(chunk_t *chunk, int fd_dst) {
	write(fd_dst, (char *) chunk->memptr, chunk->size);
}
*/
