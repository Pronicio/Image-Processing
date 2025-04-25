#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>
#include <stdio.h>

// Structure representing standard BMP header (14 bytes)
typedef struct {
    uint16_t type; // File signature (must be 0x4D42, or 'BM')
    uint32_t size; // Total size of BMP file in bytes
    uint16_t reserved1; // Reserved field (must be 0)
    uint16_t reserved2; // Reserved field (must be 0)
    uint32_t offset; // Offset (position) to the beginning of image data
} t_bmp_header;

// Structure representing image information header (40 bytes)
typedef struct {
    uint32_t size; // Size of this structure (40 bytes)
    int32_t width; // Image width in pixels
    int32_t height; // Image height in pixels
    uint16_t planes; // Number of planes (must be 1)
    uint16_t bits; // Number of bits per pixel (must be 24 here)
    uint32_t compression; // Compression type (0 = none)
    uint32_t imagesize; // Raw size of image data
    int32_t xresolution; // Horizontal resolution (pixel/meter)
    int32_t yresolution; // Vertical resolution (pixel/meter)
    uint32_t ncolors; // Number of colors in the palette (0 = all)
    uint32_t importantcolors; // Number of important colors (0 = all)
} t_bmp_info;

// Structure representing an RGB pixel (8 bits per component)
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

// Structure representing a 24-bit BMP image
typedef struct {
    t_bmp_header header; // BMP header
    t_bmp_info header_info; // Image information
    int width; // Image width (copy from header_info)
    int height; // Image height (copy from header_info)
    int colorDepth; // Color depth (must be 24)
    t_pixel **data; // Pixel matrix (image data)
} t_bmp24;

// Constants for BMP field offsets (useful for raw access)
#define BITMAP_MAGIC       0x00  // Offset of type field
#define BITMAP_SIZE        0x02  // Offset of size field
#define BITMAP_OFFSET      0x0A  // Offset of offset field (data start)
#define BITMAP_WIDTH       0x12  // Offset of width
#define BITMAP_HEIGHT      0x16  // Offset of height
#define BITMAP_DEPTH       0x1C  // Offset of color depth
#define BITMAP_SIZE_RAW    0x22  // Offset of image data size

// General constants
#define BMP_TYPE           0x4D42  // 'BM' in hexadecimal
#define HEADER_SIZE        0x0E    // Size of standard BMP header (14 bytes)
#define INFO_SIZE          0x28    // Size of BMP info header (40 bytes)
#define DEFAULT_DEPTH      0x18    // 24 bits per pixel (RGB)

t_pixel **bmp24_allocateDataPixels(int width, int height);

void bmp24_freeDataPixels(t_pixel **pixels, int height);

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);

void bmp24_free(t_bmp24 *img);

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file);

void bmp24_readPixelData(t_bmp24 *image, FILE *file);

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file);

void bmp24_writePixelData(t_bmp24 *image, FILE *file);

t_bmp24 *bmp24_loadImage(const char *filename);

void bmp24_saveImage(t_bmp24 *img, const char *filename);

void bmp24_negative(t_bmp24 *img);

void bmp24_grayscale(t_bmp24 *img);

void bmp24_brightness(t_bmp24 *img, int value);

t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize);

#endif //BMP24_H
