#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mapper.h"
#include "decompressor.h"

#define EXT_CHARS ".out"
#define CHARS_FOR_EXT 4
#define HELP_MESSAGE "\nUsage: unzip archive.zip\n"

static char *generate_dst_pathname(const char *src_pathname);

int main(int argc, char *argv[])
{
	int err_status;
	int out_fildes;
	mapper_t mapper;
	char *src_pathname, *dst_pathname;

	if (argc != 2) {
		fwrite(HELP_MESSAGE, sizeof(char), sizeof(HELP_MESSAGE), stderr);
		exit(EXIT_FAILURE);
	}

	src_pathname = argv[1];
	
	/* Инициализация "модуля", отвечающего за отображение входного файла */
	if (init_mapper(&mapper, src_pathname)) {
		exit(EXIT_FAILURE);
	}

	/* Генерируем имя выходного файла */
	dst_pathname = generate_dst_pathname(src_pathname);
	if (dst_pathname == NULL && *src_pathname) {
		close_mapper(&mapper);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s\n", dst_pathname);

	/* Создаем файл выходного потока, получаем его файловый дескриптор */
	out_fildes = open(dst_pathname, O_CREAT | O_TRUNC | O_WRONLY , 0666);
	if (out_fildes == -1) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", dst_pathname, strerror(errno));
		close_mapper(&mapper);
		// free(dst_pathname);
		exit(EXIT_FAILURE);
	}

	err_status = unzip_decompress(&mapper, out_fildes);

	if (close_mapper(&mapper)) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", src_pathname, strerror(errno));
		err_status = EXIT_FAILURE;
	}

	if (close(out_fildes)) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", dst_pathname, strerror(errno));
		err_status = EXIT_FAILURE;
	}

	//free(dst_pathname);

	return err_status ? EXIT_FAILURE : EXIT_SUCCESS;
}

static char *generate_dst_pathname(const char *src_pathname)
{
	const char *start, *ptr, *end;
	char *dst_pathname;

	if (*src_pathname == '\0') {
		return NULL;
	}

	end = src_pathname + strlen(src_pathname);
	start = end - 1;
	while (start >= src_pathname && *start != '/') {
		--start;
	}
	++start;

	for (ptr = start; *ptr; ++ptr) {
		if (*ptr == '.') {
			end = ptr;
		}
	}

	if (end == start) {
		dst_pathname = strdup("unzip.out");
		if (dst_pathname == NULL) {
			fprintf(stderr, "%s: %s: %s\n", \
							"unzip", "memory", strerror(errno));
			return NULL;
		}
	} else if (*end == '\0') {
		dst_pathname = (char *) calloc(end - start + 1 + CHARS_FOR_EXT, sizeof(char));
		if (dst_pathname == NULL) {
			fprintf(stderr, "%s: %s: %s\n", \
							"unzip", "memory", strerror(errno));
			return NULL;
		}
		strcpy(dst_pathname, start);
		strcat(dst_pathname, EXT_CHARS);
	} else {
		dst_pathname = (char *) calloc(end - start + 1, sizeof(char));
		if (dst_pathname == NULL) {
			fprintf(stderr, "%s: %s: %s\n", \
							"unzip", "memory", strerror(errno));
			return NULL;
		}
		strncpy(dst_pathname, start, end - start);
	}
	
	return dst_pathname;

}
