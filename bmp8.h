#ifndef BMP8_H
#define BMP8_H

// Structured type t_bmp8 to represent a grayscale image
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;

    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

t_bmp8 *bmp8_loadImage(const char *filename);

void bmp8_saveImage(const char *filename, t_bmp8 *img);

void bmp8_free(t_bmp8 *img);

void bmp8_printInfo(t_bmp8 *img);


#endif //BMP8_H
