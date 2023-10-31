#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "args.h"
#include "compressor.h"

/* макросы для размера статического буфера, 4 символа для расширения ".zip" */

#define BUFFER_SIZE 1024
#define CHARS_FOR_EXT 4
#define EXT_CHARS ".zip"

/* Объвления вспомогательных статических функций */

static int close_stream(FILE *stream, const char *pathname);

static char *generate_dst_pathname(const char *src_pathname);

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

/*
*  close_stream
*
*  Закрытие потока с обработкой ошибки.
*/

static int close_stream(FILE *stream, const char *pathname)
{
	if (fclose(stream)) {
		fprintf(stderr, "%s: %s: %s\n", "zip", pathname, strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*
*  generate_dst_pathname
*
*  Функция генерирует имя выходного файла, если таковой не указан пользователем,
*  отсекая всё, кроме имени входного файла, и добавляя расширение ".zip"
*  Функция по сути явяется одноразовой, поэтому вместо нормальной аллокации на куче,
*  имя файла записывается в статичный буфер размера 1 кб. Если входное имя столь
*  велико, что не помещается, то она урезается до такой длины, чтобы уместить еще и
*  символы для расширения.
*/

static char *generate_dst_pathname(const char *src_pathname)
{
	int idx;
	static char buffer[BUFFER_SIZE];

	if (*src_pathname == '\0') {
		return NULL;
	}

	idx = (int) strlen(src_pathname) - 1;
	while (idx >= 0 && src_pathname[idx] != '/') {
		--idx;
	}
	++idx;

	strncpy(buffer, src_pathname + idx, BUFFER_SIZE - CHARS_FOR_EXT - 1);
	strcat(buffer, EXT_CHARS);

	return buffer;
}
