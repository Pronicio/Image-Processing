#include "bmp24.h"

#include <stdlib.h>
#include <stdio.h>

t_pixel **bmp24_allocateDataPixels(int width, int height) {
    // Allocate the array of pointers to rows (height)
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        fprintf(stderr, "Error: failed to allocate pixel rows.\n");
        return NULL;
    }

    // Allocate each row of pixels (width)
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {
            fprintf(stderr, "Error: failed to allocate row %d.\n", i);
            // Free already allocated rows before exiting
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels == NULL) return;

    // Free each row
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }

    // Free the array of pointers
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    // Allocate the t_bmp24 structure
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fprintf(stderr, "Error: failed to allocate t_bmp24 structure.\n");
        return NULL;
    }

    // Allocate the pixel matrix
    img->data = bmp24_allocateDataPixels(width, height);
    if (img->data == NULL) {
        free(img);
        return NULL;
    }

    // Initialize image fields
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img == NULL) return;

    // Free pixels
    bmp24_freeDataPixels(img->data, img->height);

    // Free the structure itself
    free(img);
}

void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint8_t blue, green, red;
    fread(&blue, sizeof(uint8_t), 1, file);
    fread(&green, sizeof(uint8_t), 1, file);
    fread(&red, sizeof(uint8_t), 1, file);

    image->data[y][x].blue = blue;
    image->data[y][x].green = green;
    image->data[y][x].red = red;
}

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    fputc(image->data[y][x].blue, file);
    fputc(image->data[y][x].green, file);
    fputc(image->data[y][x].red, file);
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file) return;

    // Calculate padding for each line
    int padding = (4 - ((image->width * 3) % 4)) % 4;

    fseek(file, image->header.offset, SEEK_SET);

    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }

        // Skip padding bytes at the end of each line
        fseek(file, padding, SEEK_CUR);
    }
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file) return;

    // Calculate necessary padding for each line
    int padding = (4 - ((image->width * 3) % 4)) % 4;

    fseek(file, image->header.offset, SEEK_SET);

    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }

        // Write padding bytes at the end of each line
        for (int p = 0; p < padding; p++) {
            fputc(0, file);
        }
    }
}

t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: unable to open file %s\n", filename);
        return NULL;
    }

    int32_t width, height;
    uint16_t colorDepth;

    file_rawRead(BITMAP_WIDTH, &width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &colorDepth, sizeof(uint16_t), 1, file);

    if (colorDepth != DEFAULT_DEPTH) {
        fprintf(stderr, "Error: this is not a 24-bit image (depth: %d)\n", colorDepth);
        fclose(file);
        return NULL;
    }

    t_bmp24 *img = bmp24_allocate(width, height, colorDepth);
    if (img == NULL) {
        fclose(file);
        return NULL;
    }

    file_rawRead(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawRead(BITMAP_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    bmp24_readPixelData(img, file);

    fclose(file);

    return img;
}

void bmp24_saveImage(t_bmp24 *image, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: unable to open file %s\n", filename);
        return;
    }

    file_rawWrite(BITMAP_MAGIC, &image->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(BITMAP_SIZE, &image->header_info, sizeof(t_bmp_info), 1, file);

    bmp24_writePixelData(image, file);

    fclose(file);
}

void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t red = img->data[y][x].red;
            uint8_t green = img->data[y][x].green;
            uint8_t blue = img->data[y][x].blue;

            uint8_t gray = (red + green + blue) / 3;

            img->data[y][x].red = gray;
            img->data[y][x].green = gray;
            img->data[y][x].blue = gray;
        }
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Error: invalid image pointer in bmp24_brightness\n");
        return;
    }

    if (img->width <= 0 || img->height <= 0) {
        fprintf(stderr, "Error: invalid image dimensions in bmp24_brightness\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = (int) img->data[y][x].red + value;
            int g = (int) img->data[y][x].green + value;
            int b = (int) img->data[y][x].blue + value;

            img->data[y][x].red = (r > 255) ? 255 : (r < 0 ? 0 : (uint8_t) r);
            img->data[y][x].green = (g > 255) ? 255 : (g < 0 ? 0 : (uint8_t) g);
            img->data[y][x].blue = (b > 255) ? 255 : (b < 0 ? 0 : (uint8_t) b);
        }
    }
}
