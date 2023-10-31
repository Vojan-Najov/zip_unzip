#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "zlib.h"


/*
*  Объявление вспомогательных статических функций
*/

static int allocate_chunks(Bytef **in_chunk_ptr, \
														Bytef **out_chunk_ptr, size_t chunksize);

static int write_out_stream(const Bytef *buf, size_t nbytes, FILE *out_stream);

/*
* compress
* Функция, для компрессии данных с использование метода дефляции
* библиотеки zlib.
* В случае какой-либо ошибки на стандартный поток ошибок выводится сообщение,
* а возвращаемое значение не равно нулю. 
*/

int zip_compress(FILE *in_stream, FILE *out_stream, size_t chunksize)
{
	int flush;
	z_stream stream = {0};
	int err_status = EXIT_SUCCESS;
	size_t nread, ncompressed;
	Bytef *in_chunk, *out_chunk;

	/* Инициализация потока типа z_stream */
	if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
		fprintf(stderr, "%s: %s: %s\n", "zip", "zlib", stream.msg);
		return EXIT_FAILURE;
	}

	/* Аллокация динамической памяти под вх. и вых. буфер */
	/* В случае ошибки по этим адресам записан NULL, а цикл будет проигнорирован */
	err_status = allocate_chunks(&in_chunk, &out_chunk, chunksize);

	flush = Z_NO_FLUSH;
	while (flush != Z_FINISH && err_status != EXIT_FAILURE) {

		/* чтения куска данных из входного файла с обработкой ошибок */
		nread = fread(in_chunk, sizeof(char), chunksize, in_stream);
		if (ferror(in_stream)) {
			err_status = EXIT_FAILURE;
			fprintf(stderr, "%s: %s: %s\n", "zip", "input file", "read failed");
			break;
		}

		/* Если файл закончился, то уявно указываем zlib, что это последний буфер. */
		if (feof(in_stream)) {
			flush = Z_FINISH;
		}

		/* Передаем адрес куска прочит. данных и количество байт в структуру z_stream */
		stream.avail_in = nread;
		stream.next_in = in_chunk;

		do {
			/* Передаем размер и адрес начала свободной памяти выходного буфера */
			stream.avail_out = chunksize;
			stream.next_out = out_chunk;

			/* Вызов функции по сжатию входного потока */
			/* Функция deflate возвращает 2 ошибки, */
			/* но обе связаны с не правильно заполненой структурой z_stream */
			deflate(&stream, flush);

			/* Вычисляем сколько байт в выходном буфере готово к записи */
			ncompressed = chunksize - stream.avail_out;

			/* Записываем данные в выходной поток */
			err_status = write_out_stream(out_chunk, ncompressed, out_stream);
			
		} while (!stream.avail_out && err_status != EXIT_FAILURE);
		/* пока не обработан весь вых. буфер z_stream, т.е. пока он хотябы 
		 * частично заполнен, или не случилась ошибка записи */
	}

	/* Очищение ресурсов потока типа z_stream */
	if (deflateEnd(&stream) != Z_OK) {
		err_status = EXIT_FAILURE;
		fprintf(stderr, "%s: %s: %s\n", "zip", "zlib", stream.msg);
	}

	/* Очищение динамической памяти, выделенной под чанки */
	free(in_chunk);
	free(out_chunk);

	return err_status;
}

/*
* allocate_chunks
* Вспомогательная функция. Цель выделить динамическую память для
* входного и выходного буфера. В случае ошибки, в стандартный поток
* ошибок выводится сообщение, а возвращаемое значение не равно 0.
*/ 

static int allocate_chunks(Bytef **in_chunk_ptr, \
														Bytef **out_chunk_ptr, size_t chunksize)
{
	*in_chunk_ptr = (Bytef *) malloc(sizeof(Bytef) * chunksize);
	if (*in_chunk_ptr == NULL) {
		*out_chunk_ptr = NULL;
		fprintf(stderr, "%s: %s: %s\n", "zip", "memory", strerror(errno));
		return EXIT_FAILURE;
	}

	*out_chunk_ptr = (Bytef *) malloc(sizeof(Bytef) * chunksize);
	if (*out_chunk_ptr == NULL) {
		free(*in_chunk_ptr);
		*in_chunk_ptr = NULL;
		fprintf(stderr, "%s: %s: %s\n", "zip", "memory", strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
*  write_out_stream
*
*  Функция записи в файл. Запись проходит в цикле, пока все nbytes байтов
*  не будет записано файл, или случится ошибка записи.
*/

static int write_out_stream(const Bytef *buf, size_t nbytes, FILE *out_stream)
{
	size_t nwrite;
	size_t offset;

	offset = 0;
	do {
		nwrite = fwrite(buf + offset, sizeof(char), nbytes, out_stream);
		if (ferror(out_stream)) {
			fprintf(stderr, "%s: %s: %s\n", "zip", "output file", "write failed");
			return EXIT_FAILURE;
		}	else {
			offset += nwrite;
			nbytes -= nwrite;
		}
	} while (nbytes > 0);

	return EXIT_SUCCESS;
}

