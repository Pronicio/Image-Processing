#ifndef BMP8_H
#define BMP8_H

// Structured type t_bmp8 to represent a grayscale image
typedef struct {
    unsigned char header[54]; // BMP header
    unsigned char colorTable[1024]; // Color palette (256 colors, 4 bytes each)
    unsigned char *data; // Pixel data

    unsigned int width; // Image width
    unsigned int height; // Image height
    unsigned int colorDepth; // Color depth (should be 8 bits)
    unsigned int dataSize; // Size of the pixel data (rowSize * height)
} t_bmp8;

// Function prototypes for BMP8 image processing
t_bmp8 *bmp8_loadImage(const char *filename);
void bmp8_saveImage(const char *filename, t_bmp8 *img);
void bmp8_free(t_bmp8 *img);
void bmp8_printInfo(t_bmp8 *img);

// Simple modifications
void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(const t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);

// Apply a filter to a BMP8 image using a kernel
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);


#endif //BMP8_H
