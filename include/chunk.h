#ifndef ZIP_UNZIP_CHUNK_H_
#define ZIP_UNZIP_CHUNK_H_

#include <stddef.h>

typedef struct chunk {
	unsigned char *ptr;
	size_t size;
} chunk_t;

#endif  /* ZIP_UNZIP_CHUNK_H_ */
