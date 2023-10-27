#ifndef ZIP_UNZIP_PARAMS_H_
#define ZIP_UNZIP_PARAMS_H_

#include <stddef.h>

#define PARAMS_NO_ERROR 0
#define PARAMS_ERROR 1

#define HELP_MESSAGE "\nHelp message not implemented.\n"

typedef struct params {
	size_t chunksize;
	const char *src_pathname;
	const char *dst_pathname;
} params_t;

int parse_params(int argc, char *argv[], params_t *params);

#endif  /* ZIP_UNZIP_PARAMS_H_ */
