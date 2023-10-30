#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "params.h"
#include "mapper.h"

//int zipping_chunk(...);

int main(int argc, char *argv[])
{
	size_t nwrite;
	chunk_t chunk;
	chunk_t out_buffer;
	params_t params;
	mapper_t mapper;
	FILE *out_stream;
	int err_status = 0;

	/* Разбор аргументов командной строки */
	err_status = parse_params(argc, argv, &params);
	if (err_status == PARAMS_ERROR) {
		fwrite(HELP_MESSAGE, sizeof(char), sizeof(HELP_MESSAGE), stderr);
		exit(EXIT_FAILURE);
	}

	/* Если выходной файл не указан, генереруем его имя */
	if (params.dst_pathname == NULL) {
		params.dst_pathname = generate_zip_pathname(params.src_pathname);
	}
	
	/* Инициализация "модуля", отвечающего за отображние входного файла */
	err_status = init_mapper(&mapper, params.src_pathname, 0);
	if (err_status) {
		fprintf(stderr, "%s: %s '%s': %s\n", \
						argv[0], "mapping", params.src_pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Открываем поток выходного файла */
	out_stream = fopen(params.dst_pathname, "w");
	if (out_stream == NULL) {
		fprintf(stderr, "%s: %s: %s\n", \
						argv[0], params.dst_pathname, strerror(errno));
		close_mapper(&mapper);
		exit(EXIT_FAILURE);
	}

	/* Инициализируем буфер, куда записываем архивированные данные */
	err_status = allocate_chunk(&out_buffer, mapper.chunksize);
	if (err_status) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		close_mapper(&mapper);
		fclose(out_stream);
		exit(EXIT_FAILURE);
	}

	while (next_chunk(&mapper)) {
		err_status = pop_chunk(&mapper, &chunk);
		if (err_status) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			close_mapper(&mapper);
			fclose(out_stream);
			free_chunk(&out_buffer);
			exit(EXIT_FAILURE);
		}

		// zipping_chunk(&chunk, &out_buffer);

		nwrite = fwrite(chunk.memptr, sizeof(char), out_buffer.size, out_stream);
		if (nwrite != out_buffer.size) {
			fprintf(stderr, "70%s: %s: %s\n", \
							argv[0], params.dst_pathname, strerror(errno));
			close_mapper(&mapper);
			put_chunk(&chunk);
			fclose(out_stream);
			free_chunk(&out_buffer);
			exit(EXIT_FAILURE);
		}
	
		err_status = put_chunk(&chunk);
		if (err_status) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			close_mapper(&mapper);
			fclose(out_stream);
			free_chunk(&out_buffer);
			exit(EXIT_FAILURE);
		}
	}

	/* Освобождаем буфер для выходных данных */
	free_chunk(&out_buffer);

	/* Закрываем поток выходного файла */
	if (fclose(out_stream)) {
		fprintf(stderr, "%s: %s: %s\n", \
						argv[0], params.dst_pathname, strerror(errno));
		err_status = EXIT_FAILURE;
	}

	/* Вызов функции очистки ресурсов у "модуля" mapper */
	close_mapper(&mapper);

	return err_status;
}

