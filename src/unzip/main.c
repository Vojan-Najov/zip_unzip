#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mapper.h"
#include "decompressor.h"

/* макросы для расширения к имени выходного файла */

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

	/* Формат запуска предполагает один аргумент и именем архива */
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

	/* fprintf(stderr, "%s\n", dst_pathname); */

	/* Создаем файл выходного потока, получаем его файловый дескриптор */
	out_fildes = open(dst_pathname, O_CREAT | O_TRUNC | O_WRONLY , 0666);
	if (out_fildes == -1) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", dst_pathname, strerror(errno));
		close_mapper(&mapper);
		free(dst_pathname);
		exit(EXIT_FAILURE);
	}

	/* Маппинг входного файла в память чанками, декомпрессия и запись в вых. дескр. */
	err_status = unzip_decompress(&mapper, out_fildes);

	/* Освобождаем ресурсы заняты внутри маппера */
	if (close_mapper(&mapper)) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", src_pathname, strerror(errno));
		err_status = EXIT_FAILURE;
	}

	/* Закрываем дескриптор выходного файла */
	if (close(out_fildes)) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", dst_pathname, strerror(errno));
		err_status = EXIT_FAILURE;
	}

	/* Освобождение пвмяти, аллоцированнной под имя выходного файла */
	free(dst_pathname);

	return err_status;
}

/*
*  generate_dst_pathname
*
*  Функция создает копию имени входного файла, отбрасывая путь
*  и расширение ".zip". Если расширения нет, то добавляется
*  расширени ".out". Возвращаемая строка должна быть освобождена
*  вызывающей стороной. В случае ошибки возвращается NULL.
*/

static char *generate_dst_pathname(const char *src_pathname)
{
	const char *start, *ptr, *end;
	char *dst_pathname;

	if (*src_pathname == '\0') {
		return NULL;
	}

	/* Указатель end указывает на терминирующий ноль символ */
	end = src_pathname + strlen(src_pathname);

	/* Старт после цикла будет указывать на первый символ относящийся к имени */
	start = end - 1;
	while (start >= src_pathname && *start != '/') {
		--start;
	}
	++start;

	/* Теперь end указывает на последнюю точку в имени файла */
	for (ptr = start; *ptr; ++ptr) {
		if (*ptr == '.') {
			end = ptr;
		}
	}

	/* Если расширение не ".zip", end снова сдвигаем до конца строки */
	if (strcmp(end, ".zip")) {
		while (*end) {
			++end;
		}
	}

	if (end == start) {
		/* Cлучай, когда например имя было .zip */

		dst_pathname = strdup("unzip.out");
		if (dst_pathname == NULL) {
			fprintf(stderr, "%s: %s: %s\n", \
							"unzip", "memory", strerror(errno));
			return NULL;
		}
	} else if (*end == '\0') {
		/* Случай когда расширения ".zip" не найдено в имени файла */

		dst_pathname = (char *) calloc(end - start + CHARS_FOR_EXT + 1, sizeof(char));
		if (dst_pathname == NULL) {
			fprintf(stderr, "%s: %s: %s\n", \
							"unzip", "memory", strerror(errno));
			return NULL;
		}
		strcpy(dst_pathname, start);
		strcat(dst_pathname, EXT_CHARS);
	} else {
		/* Случай, когда расширение ".zip" найдено */

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

