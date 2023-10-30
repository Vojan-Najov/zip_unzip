#include <string.h>

#define BUFFER_SIZE 1024
#define CHARS_FOR_EXT 4
#define EXT_CHARS ".zip"

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

char *generate_dst_pathname(const char *src_pathname)
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
