
#include "params.h"
#include "mapper.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
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
	if (params.dst_pathname == NULL) {
		params.dst_pathname = generate_zip_pathname(params.src_pathname);
	}
	
	err_status = init_mapper(&mapper, params.src_pathname, 0);
	if (err_status) {
		fprintf(stderr, "%s: %s %s: %s\n", \
						argv[0], "mapping", params.src_pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (next_chunk(&mapper)) {
		err_status = pop_chunk(&mapper, &chunk);
		if (err_status) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			exit(EXIT_FAILURE);
		}

		fwrite(chunk.memptr, sizeof(char), chunk.size, stdout);

		err_status = put_chunk(&chunk);
		if (err_status) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

