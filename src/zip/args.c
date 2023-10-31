#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "args.h"

/*
*  Макросы соответствующие вариантам опции -m
*/

#define MEASURE_BYTE 1L
#define MEASURE_KIBIBYTE 1024L
#define MEASURE_MIBIBYTE (1024L * 1024L)
#define MEASURE_GIBIBYTE (1024L * 1024L * 1024L)

/* Сообщения об ошибках представлены в виде макросов */

#define MEASURE_ERROR_MSG \
"option -m: unknown unit of mesuare: expected B, KB, MB or GB"

#define SIZE_ERROR_MSG \
"option -s: incorrect argument: expected unsigned integer"

#define PATHNAME_ERROR_MSG \
"the path to the file is expected"

#define EXTRA_ARGS_MSG \
"extra parameters detected"

#define SIZE_OVERFLOW_MSG \
"the chunk size is too big"

#define FILE_TYPE_ERROR \
"not a regular file" 

/*
*  Объявления вспомагательных для parse_args статичных функций
*/

static int parse_options(int argc, char *argv[], size_t *size);

static int parse_pathname(int index, char *argv[], \
                          char **src_pathname, char **dst_pathname);

/*
*  parse_args
*
*  Сначала обрабатываются короткие опции, если они есть.
*  Затем из массива аргументов выбираются пути до входного файла (обязательно)
*  и до выходного файла (необязательно)
*  Также функция принимает адрес структуру типа args_t, которая содержит поля:
*   - src_pathname, путь до исходного файла (обязательный аргумент)
*   - dst_pathname, путь до выходного файла (не обязательный аргумент, который будет 
*     сгенерирован в случае отсутствия)
*   - chunksize, размер чанка равный размеру, переданному через опцию s, и
*     приведенному к байтам домножением на коэфициет, если задана опция m. Если опции
*     не заданы, то размер чанка инициализируется значением по-умолчнию.
*  В случае ошибки на стандартный поток ошибок выводится соотв. сообщение,
*  а возвращаемое значение не равно 0.
*/

int parse_args(int argc, char *argv[], args_t *args) {
	int index;

	index = parse_options(argc, argv, &args->chunksize);
	if (index < 0) {
		return EXIT_FAILURE;
	}

	if (parse_pathname(index, argv, &args->src_pathname, &args->dst_pathname)) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
*  parse_options
*
*  Разбор коротких опций, которые задают размер чанка для чтения/компрессии.
*  Поддерживается следующие опции: 
*   - h, вывести сообщение об опциях.
*   - m, которая служит, чтобы указать единицу измерения байт (B), кибибайт (KB),
*     мибибайт (MB), гибибайт (GB).
*   - s, которая служит, чтобы указать размер чанка; ожидается натуральное число.
*  Изспользуется стандартная функция getopt.
*  По-умолчанию мерой чанка является байт, а размер 4096.
*  Когда getopt возвращает опцию, которая содержится в строке кодирующей опции,
*  начало значения опции находится по адресу optarg.
*  В случае успеха возвращается индекс первого элемента массива argv,
*  который не является короткой опцией, это значение находится в optind,
*  a по адресу sizeptr находится размер чанка.
*  В случае ошибки на стандартный поток ошибок выводится соотв. сообщение,
*  а возвращаемое значение является отрицательным числом.
*/

static int parse_options(int argc, char *argv[], size_t *sizeptr) {
	int option;
	char *endptr;
	long int size = DEFAULT_CHUNKSIZE;
	long int measure = MEASURE_BYTE;

	while ((option = getopt(argc, argv, "hm:s:")) != -1) {
		switch (option) {
			case 'h':
				fwrite(HELP_MESSAGE, sizeof(char), sizeof(HELP_MESSAGE), stderr);
				exit(EXIT_SUCCESS);
				break;
			case 'm':
				if (!strcasecmp(optarg, "B")) {
					measure = MEASURE_BYTE;
				} else if (!strcasecmp(optarg, "KB")) {
					measure = MEASURE_KIBIBYTE;
				} else if (!strcasecmp(optarg, "MB")) {
					measure = MEASURE_MIBIBYTE;
				} else if (!strcasecmp(optarg, "GB")) {
					measure = MEASURE_GIBIBYTE;
				} else {
					fprintf(stderr, "%s: %s\n", "zip", MEASURE_ERROR_MSG);
					return -1;
				}
				break;
			case 's':
				size = strtol(optarg, &endptr, 10);
				/*
				* в случае переполнения strtol возвращает LONG_MAX, т.к. это число и так 
				* достаточно велико, то доп. проверки на значение errno не проводится. 
				*/
				if (size <= 0 || *endptr != '\0') {
					fprintf(stderr, "%s: %s\n", "zip", SIZE_ERROR_MSG);
					return -1;
				} else if (size == LONG_MAX) {
					fprintf(stderr, "%s: %s\n", "zip", SIZE_OVERFLOW_MSG);
					return -1;
				}
				break;
			case '?':
				return -1;
		}
	}

	/* Пробуем отловить переполнение произведения size * measure */
	if (size > LONG_MAX / measure) {
		fprintf(stderr, "%s: %s\n", "zip", SIZE_OVERFLOW_MSG);
		return -1;
	}

	/* fprintf(stderr, "size = %zu\n", size * measure); */

	*sizeptr = size * measure;

	return optind;
}

/*
*  parse_pathname
*
*  index указывает на первый не опциональный аргумент.
*  Если все успешно, то по по адресу src_pathname_ptr будет находится
*  адрес строки первго аргумента. А если указан путь для выходного файла
*  по адресу dst_pathname_ptr адрес строки вторго вргумента, иначе там будет
*  NULL ( а  имя файла будет сгенерировано позже в вызывающей функции).
*  Если массив аргументов на этом не закончился, сообщаем об ошибке пользователю.
*/

static int parse_pathname(int index, char *argv[], \
                          char **src_pathname_ptr, char **dst_pathname_ptr)
{
	struct stat statbuf;

	/*
	 * Если индекс указывает, на последний элемент массива аргументов,
   * или элемент является пустой строкой.
	 * значит пользователь не ввел путь до файла.
	*/
	if (argv[index] == NULL || argv[index][0] == '\0') {
		fprintf(stderr, "%s: %s\n", "zip", PATHNAME_ERROR_MSG);
		return EXIT_FAILURE;
	}
	
	*src_pathname_ptr = argv[index];

	/* Проверка, что файл является регулярным (regular) */
	if (stat(*src_pathname_ptr, &statbuf)) {
		fprintf(stderr, "%s: %s: %s\n", \
						"zip", *src_pathname_ptr, strerror(errno));
		return EXIT_FAILURE;
	}
	if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: %s: %s\n", \
						"zip", *src_pathname_ptr, FILE_TYPE_ERROR);
		return EXIT_FAILURE;
	}

	/* Если больше аргументов нет, имя выходного файла сгенерируем позже. */
	if (argv[++index] == NULL) {
		*dst_pathname_ptr = NULL;
	} else {
		*dst_pathname_ptr = argv[index];

		/* Теперь, если остались еще аргументы, выведем сообщение об ошибке. */
		if (argv[++index] != NULL) {
			fprintf(stderr, "%s: %s\n", argv[0], EXTRA_ARGS_MSG);
			return EXIT_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
} 

