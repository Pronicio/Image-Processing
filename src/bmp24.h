#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// File header (14 bytes)
typedef struct {
    uint16_t type;       // Signature 'BM'
    uint32_t size;       // File size in bytes
    uint16_t reserved1;  // Reserved, must be 0
    uint16_t reserved2;  // Reserved, must be 0
    uint32_t offset;     // Offset to pixel data
} t_bmp_header;

// Info header (BITMAPINFOHEADER, 40 bytes)
typedef struct {
    uint32_t headerSize;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPelsPerMeter;
    int32_t  yPelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} t_bmp_info;

// Pixel structure (BGR)
typedef struct {
    uint8_t B;
    uint8_t G;
    uint8_t R;
} t_pixel;

// 24-bit BMP image structure
typedef struct {
    t_bmp_header fileHeader;
    t_bmp_info   infoHeader;
    t_pixel    **data;      // 2D array [height][width]
    int          width;
    int          height;
    int          padding;   // Row padding in bytes
} t_bmp24;

// Core I/O and memory management
t_bmp24 *bmp24_loadImage(const char *filename);
int     bmp24_saveImage(const char *filename, t_bmp24 *img);
void    bmp24_freeImage(t_bmp24 *img);
void    bmp24_printInfo(const t_bmp24 *img);

// Pixel data helpers
t_pixel **bmp24_allocateDataPixels(int width, int height);
void     bmp24_freeDataPixels(t_pixel **pixels, int height);
void     bmp24_readPixelData(FILE *fp, t_bmp24 *img);
void     bmp24_writePixelData(FILE *fp, const t_bmp24 *img);

// Image operations
void bmp24_grayscale(t_bmp24 *img);
void bmp24_brightness(t_bmp24 *img, int value);
void bmp24_negative(t_bmp24 *img);
t_pixel bmp24_convolution(const t_bmp24 *img, int x, int y, const float *kernel, int kernelSize);
void    bmp24_applyFilter(t_bmp24 *img, const float *kernel, int kernelSize);

// Predefined 3×3 filters
void bmp24_boxBlur(t_bmp24 *img);
void bmp24_gaussianBlur(t_bmp24 *img);
void bmp24_outline(t_bmp24 *img);
void bmp24_emboss(t_bmp24 *img);
void bmp24_sharpen(t_bmp24 *img);

#endif // BMP24_H
