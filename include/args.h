#ifndef ZIP_ARGS_H_
#define ZIP_ARGS_H_

#include <stddef.h>

#define DEFAULT_CHUNKSIZE 4096

#define HELP_MESSAGE \
"\nUsage: zip [options] filename [archive.zip]\n" \
"Options:\n" \
"-m  --  the measure of chunk's measurement.\n" \
"        Acceptable values are B, KB, MB, GB, which corresponds to\n" \
"        bytes, kibibytes, mebibytes and gibibbytes. Default measure is byte.\n" \
"-s  --  explicitly specify the size of the chunk.\n" \
"        A non-negative integer is expected.\n" \
"If there is no output file name, it is generated\n" \
"by adding a \".zip\" postfix to the input file name.\n"

typedef struct args {
	size_t chunksize;
	char *src_pathname;
	char *dst_pathname;
} args_t;

int parse_args(int argc, char *argv[], args_t * args);

#endif  /* ZIP_ARGS_H_ */
