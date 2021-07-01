#ifndef __LIBSECRETEBMP_H
#define __LIBSECRETEBMP_H

// Original author had these const ints.
#define BYTES_PER_PIXEL  ((int)3)   // red, green, & blue
#define FILE_HEADER_SIZE ((int)14)
#define INFO_HEADER_SIZE ((int)40)

#ifdef __cplusplus
extern "C" {
#endif

void generateBitmapImageByRow(
		unsigned char* (*cb_row)(void *cbdata), // b g r
		void (*cb_writer)(
			unsigned char *data, int size, int nmemb, void *cbdata),
		void (*cb_done)(void *cbdata),
		void *cbdata, // Optional caller-managed data (NULL if none)
		int height,
		int width
	);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);

#ifdef __cplusplus
}
#endif

#ifndef __IN_LIBSECRETEBMP_C
#endif  // / __IN_LIBSECRETEBMP_C

#endif  // __LIBSECRETEBMP_H
