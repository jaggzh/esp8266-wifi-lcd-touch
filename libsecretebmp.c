#include <stdio.h>
#include "libsecretebmp.h"

void generateBitmapImageByRow(
		unsigned char* (*cb_row)(void *cbdata),
		void (*cb_writer)(
			unsigned char *data, int size, int nmemb, void *cbdata),
		void (*cb_done)(void *cbdata),
		void *cbdata,
		int height,
		int width) {
    int widthInBytes = width * BYTES_PER_PIXEL;
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    int stride = (widthInBytes) + paddingSize;
    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    (*cb_writer)(fileHeader, 1, FILE_HEADER_SIZE, cbdata);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    (*cb_writer)(infoHeader, 1, INFO_HEADER_SIZE, cbdata);

    int i;
    for (i = 0; i < height; i++) {
    	unsigned char *rowd;
    	rowd = (*cb_row)(cbdata); // request row of data
		#ifdef SECRETE_BMP_DEBUG
			if (rowd == NULL) {
				fprintf(stderr, "cb_row() returned NULL\n");
				exit(1);
			}
		#endif
		(*cb_writer)(rowd, BYTES_PER_PIXEL, width, cbdata);
		(*cb_writer)(padding, 1, paddingSize, cbdata);
    }
    if (cb_done) (*cb_done)(cbdata);
}

unsigned char* createBitmapFileHeader (int height, int stride) {
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);
    printf("Filesize: %d\n", fileSize);
    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };
    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);
    //fileHeader[0] = 0xAA;     // was debugging headers
    return fileHeader;
}

unsigned char* createBitmapInfoHeader (int height, int width) {
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };
    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);
    //infoHeader[0] = 0xBB;       // was debugging headers
    //infoHeader[30-14] = 0x88;
    //infoHeader[31-14] = 0x99;
    return infoHeader;
}
