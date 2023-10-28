#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "params.h"

/*
* Здесь представлены функции, предназначенные для разбора аргументов.
* Поддерживается следующие опции:
*   - h, вывести сообщение об опциях.
*   - m, которая служит, чтобы указать единицу измерения байт (B), кибибайт (KB),
*     мибибайт (MB), гибибайт (GB). По умолчанию считается, что размер чанка в байтах.
*   - s, которая служит, чтобы указать размер чанка; ожидается натуральное число.
* На вход принимается массив аргументов командной строки и количество аргументов.
* Также функция принимает адрес структуру типа params_t, которая содержит поля:
*   - src_pathname, путь до исходного файла (обязательный аргумент)
*   - dst_pathname, путь до выходного файла (не обязательный аргумент, который будет 
*     сгенерирован в случае отсутствия)
*   - chunksize, размер чанка равный размеру, переданному через опцию s, и
*     приведенному к байтам домножением на коэфициет, если задана опция m. Если опции
*     не заданы, то размер чанка инициализируется значением по-умолчнию далее по 
*     выполнению программы.
* При успешном завершении, на выходе структура типа params_t будет проинициализирова-
* на, и в качестве статуса будет возвращен 0.
* В противном случае функция возвращает код ошибки, не равный нулю.
*/

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

/*
*  Объявления вспомагательных для parse_params статичных функций
*/

static int parse_options(int argc, char *argv[], size_t *size);

static int parse_pathname(int index, char *argv[], \
                          char **src_pathname, char **dst_pathname);

/*
*  parse_params
*  Основная функция разбора аргументов.
*  После вызова parse_options, index содержит индекс первого не опционального
*  аргумента из массива argv. В случае ошибки, index отрицательный.
*  В случае успеха, инициализировано поле chunksize структуры типа params_t.
*  
*  Затем обрабатываем имена файлов (которых должно быть не меньше одного).
*
*  Если разбор аргументов прошел успешно возвращается значение PARAMS_NO_ERROR,
*  а структура типа params_t проинициализирована.
*  Иначе вызывающая функция, получит код ошибки.
*  Все сообщения об ошибках выводят вспомагательные функции.
*/

int parse_params(int argc, char *argv[], params_t *params) {
	int index;
	long int size = 0;
	int err_status = PARAMS_NO_ERROR;

	if ((index = parse_options(argc, argv, &params->chunksize)) < 0) {
		return PARAMS_ERROR;
	}

	err_status = parse_pathname(index, argv, \
                              &params->src_pathname, &params->dst_pathname);

	return err_status;
}

/*
* parse_options
*
* Вспомагательная функция разбора опций. Используется стандартная функция getopt
* для работы с короткими опциями.
* Функция возвращает индекс аргументов, на которых закончился разбор, т.е. по этому
* индексу лежит первый аргумент, который не является короткой опцией.
* В случае ошибки возращается отрицательное число, а на стандартный поток ошибок
* выводится сообщение об ошибке.
* В случае успеха по адресу sizeptr находится размер чанка.
*/

static int parse_options(int argc, char *argv[], size_t *sizeptr) {
	int option;
	char *endptr;
	long int size;
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
					fprintf(stderr, "%s: %s\n", argv[0], MEASURE_ERROR_MSG);
					return -1;
				}
				break;
			case 's':
				size = strtol(optarg, &endptr, 10);
				/*
				* в случае переполнения strtol возвращает LONG_MAX, так это число и так 
				* достаточно велико, то доп проверки на значение errno, не проводится. 
				*/
				if (size < 0 || *endptr != '\0') {
					fprintf(stderr, "%s: %s\n", argv[0], SIZE_ERROR_MSG);
					return -1;
				} else if (size == LONG_MAX) {
					fprintf(stderr, "%s: %s\n", argv[0], SIZE_OVERFLOW_MSG);
					return -1;
				}
				break;
			case '?':
				return -1;
		}
	}

	/* Пробуем перемножить размер и размерность, в случае переполнения выдаем ошибку */
	if ((unsigned long)size * (unsigned long)measure > (unsigned long)LONG_MAX) {
		fprintf(stderr, "%s: %s\n", argv[0], SIZE_OVERFLOW_MSG);
		return -1;
	}

	*sizeptr = size * measure;

	return optind;
}

/*
*  parth_pathname
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
	/*
	 * Если индекс указывает, на последний элемент массива аргументов,
	 * значит пользователь не ввел путь до файла.
	*/
	if (argv[index] == NULL || argv[index][0] == '\0') {
		fprintf(stderr, "%s: %s\n", argv[0], PATHNAME_ERROR_MSG);
		return PARAMS_ERROR;
	}
	
	*src_pathname_ptr = argv[index];

	/* Если больше аргументов нет, имя выходного файла сгенерируем позже. */
	if (argv[++index] == NULL) {
		*dst_pathname_ptr = NULL;
	} else {
		*dst_pathname_ptr = argv[index];

		/* Теперь, если остались еще аргументы, выведем сообщение об ошибке. */
		if (argv[++index] != NULL) {
			fprintf(stderr, "%s: %s\n", argv[0], EXTRA_ARGS_MSG);
			return PARAMS_ERROR;
		}
	}

	return PARAMS_NO_ERROR;
} 

