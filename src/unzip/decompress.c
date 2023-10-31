#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "zlib.h"
#include "mapper.h"

static int write_out_fildes(const Bytef *buf, size_t nbytes, int fildes);

/*
*  unzip_decompress
*
*  Отображение файла в память, декомпрессия с использование метода inflate 
*  библиотеки  zlib и запись в файловый дескриптор выходного файла.
*/

int unzip_decompress(mapper_t *mapper, int out_fildes)
{
	int zstatus;
	int err_status;
	size_t ndecompressed;
	chunk_t chunk;
	Bytef *out_buffer;
	z_stream stream = {0};

	/* Инициализация структуры типа z_stream */
	err_status = inflateInit(&stream);
	if (err_status != Z_OK) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "zlib", stream.msg);
		return EXIT_FAILURE;
	}

	/* Аллокация буфера для декомпрессированных данных */
	out_buffer = (Bytef *) malloc(sizeof(Bytef) * mapper->chunksize);
	if (out_buffer == NULL) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "memory", strerror(errno));
		err_status = EXIT_FAILURE;
	}

	/* Пока не отображен весь файл, цикл чтения декомпрессии и записи */
	while (next_chunk(mapper) && !err_status) {

		/* Отображаем очередной чанк */
		err_status = pop_chunk(mapper, &chunk);
		if (err_status) {
			break;
		}

		/* Инициализируем поля структуры z_stream, входной буфер и его размер */
		stream.avail_in = chunk.size;
		stream.next_in = chunk.ptr;

		do {
			/* Инициализируем поля структуры z_stream, выходной буфер и его размер */
			stream.avail_out = mapper->chunksize;
			stream.next_out = out_buffer;

			/* Вызов функции декомпрессии */
			zstatus = inflate(&stream, Z_NO_FLUSH);
			if (zstatus != Z_OK && zstatus != Z_STREAM_END) {
				fprintf(stderr, "%s: %s: %s\n", "unzip", "zlib", stream.msg);
				err_status = EXIT_FAILURE;
				break;
			}

			/* Вычисляем размер данных готовых для записи */
			ndecompressed = mapper->chunksize - stream.avail_out;
			
			/* запись в выходной файл */
			err_status = write_out_fildes(out_buffer, ndecompressed, out_fildes);

		} while (!stream.avail_out && !err_status);
		/* Пока в выходном буфере, есть что записывать */
		/* Когда будет обработан весь входной буфер, inflate  вернет Z_STREAM_END */
		/* а значение avail_out обнулится */

		/* Освобождаем отображаемую память */
		put_chunk(&chunk);
	}

	if (inflateEnd(&stream) != Z_OK) {
		err_status = EXIT_FAILURE;
		fprintf(stderr, "%s: %s: %s\n", "zip", "zlib", stream.msg);
	}

	free(out_buffer);

	return err_status;
}

/*
*  write_out_fildes
*
*  Циклично записывает буфер в файл, пока все данные не будут записаны или не
*  случится ошибка
*/

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


