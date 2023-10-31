#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mapper.h"

/* Количество страниц виртуальной памяти, отведенных под "маппирование" файла */
#define DEFAULT_NPAGE 4


/*
*  init_mapper
*
*  Инициализия структуры mapper_t.
*  Меня немного сбило с толку задание, где просят "подмаппировать" файл.
*  Поэтому для unzip, где не предусмутрены опции определения размера чанка,
*  я выбрал способ отображения файла прямо в виртуальную память процесса.
*  Т.к. нужно выравнивать смещение по файлу относительно размера страницы
*  этот способ чтения файла не совсем подходит для программы zip.
*  Или бы пришлось выравнивать размер чанка под размер страницы.
*  Если в zip, работа с файлами идет средствами стандартной библиотеки C stdio,
*  То в unzip используются только системные вызовы posix
*
*  Если произошла ошибка, на стандартный поток ошибок выводится сообщение об этом.
*  И возвращается не нулевой код возврата.
*/

int init_mapper(mapper_t *mapper, const char *pathname)
{
	struct stat stat_buffer;
	int err_status = MAPPER_NO_ERROR;

	/* Получаем информацию о файле */
	err_status = stat(pathname, &stat_buffer);
	if (err_status) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, strerror(errno));
		return MAPPER_STATFILE_ERROR;
	}
	/* Если файл не является регулярным (regular), то выдаем ошибку */
	if ((stat_buffer.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, "not a regular file");
		return MAPPER_REGFILE_ERROR;
	}

	/* Получаем размер файла */
	mapper->filesize = stat_buffer.st_size;

	/* Проверка, что файл не пуст */
	if (!mapper->filesize) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, "empty file");
		return MAPPER_EMPTYFILE_ERROR;
	}

	/* размер чанка быбран как 4 страницы */
	mapper->chunksize = DEFAULT_NPAGE * getpagesize();
	
	/* Смещение по файлу */
	mapper->offset = 0;

	/* Открываем файловый дескрипток для входного файла */
	mapper->fildes = open(pathname, O_RDONLY);
	if (mapper->fildes == -1) {
		fprintf(stderr, "%s: '%s': %s\n", "unzip", pathname, strerror(errno));
		return MAPPER_OPENFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

/*
*  close_mapper
*
*  Закрываем файловый дескриптор отображаемого файла
*/

int close_mapper(mapper_t *mapper) {
	if (close(mapper->fildes)) {
		return MAPPER_CLOSEFILE_ERROR;
	}

	return MAPPER_NO_ERROR;
}

/*
*  next_chunk
*
*  Проверка, что отображение файла не закончено
*  Пока offset муньше filesize, есть что отображать
*/

int next_chunk(mapper_t *mapper)
{
	return mapper->filesize > mapper->offset;
}

/*
*  pop_chunk
*
*  Отобразить очередной чанк файла в память процесса
*/

int pop_chunk(mapper_t *mapper, chunk_t *chunk) {
	size_t len;
	int err_status = MAPPER_NO_ERROR;

	/* Если размер чанка больше остатка, отображаем только остаток */
	/* Иначе отображаем весь размер чанка */
	if (mapper->filesize - mapper->offset >= mapper->chunksize) {
		len = mapper->chunksize;
	} else {
		len = mapper->filesize - mapper->offset;
	}

	/* chunk_t это просто буфер и размер в одной структуре */
	chunk->ptr = (unsigned char *) mmap(NULL, len, PROT_READ, MAP_SHARED, \
                                      mapper->fildes, mapper->offset);

	/* Если ошибка, обрабытываем */
	/* Иначе увелечиваем смещение по файлу и устанавливаем размер чанка */
	if (chunk->ptr == MAP_FAILED) {
		fprintf(stderr, "%s: %s: %s\n", "unzip", "mapping", strerror(errno));
		err_status = MAPPER_MAP_ERROR;
		chunk->size = 0;
	} else {
		chunk->size = len;
		mapper->offset += len;
	}

	return err_status;
}


/*
*  put_chunk
*
*  Возвращаем ресурсы
*/

int put_chunk(chunk_t *chunk) {
	int err_status = MAPPER_NO_ERROR;

	if (chunk->ptr != MAP_FAILED) {
		munmap(chunk->ptr, chunk->size);
	} else {
		err_status = MAPPER_MAP_ERROR;
	}

	return err_status;
}

