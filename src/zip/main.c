#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "args.h"
#include "compressor.h"

static int close_stream(FILE *stream, const char *pathname);

int main(int argc, char *argv[])
{
	int err_status;
	args_t args = {0};
	FILE *in_stream, *out_stream;
	
	/* Разбор аргументов командной строки */
	err_status = parse_args(argc, argv, &args);
	if (err_status) {
		fwrite(HELP_MESSAGE, sizeof(char), sizeof(HELP_MESSAGE), stderr);
		exit(EXIT_FAILURE);
	}

	/* Если выходной файл не указан, генерируем его имя */
	if (args.dst_pathname == NULL) {
		args.dst_pathname = generate_dst_pathname(args.src_pathname);
	}

	/* Открываем входной и выходной потоки */
	in_stream = fopen(args.src_pathname, "r");
	if (in_stream == NULL) {
		fprintf(stderr, "%s: %s: %s\n", "zip", args.src_pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}
	out_stream = fopen(args.dst_pathname, "w");
	if (in_stream == NULL) {
		fprintf(stderr, "%s: %s: %s\n", "zip", args.src_pathname, strerror(errno));
		close_stream(in_stream, args.dst_pathname);
		exit(EXIT_FAILURE);
	}

	/* Чтения входного потока, компрессия и запись в выходной поток */
	err_status = zip_compress(in_stream, out_stream, args.chunksize);

	/* Закрываем входной и выходной потоки */
	err_status += close_stream(in_stream, args.src_pathname);
	err_status += close_stream(out_stream, args.dst_pathname);

	return err_status ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int close_stream(FILE *stream, const char *pathname)
{
	if (fclose(stream)) {
		fprintf(stderr, "%s: %s: %s\n", "zip", pathname, strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
