#ifndef ZIP_COMPRESSOR_H_
#define ZIP_COMPRESSOR_H_

int zip_compress(FILE *in_stream, FILE *out_stream, size_t chunksize);

#endif  /* ZIP_COMPRESSOR_H_ */
