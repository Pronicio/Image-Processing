#include "bmp24.h"
#include <stdint.h>
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

// Libération de la matrice de pixels
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels) {
        for (int i = 0; i < height; i++) {
            free(pixels[i]);
        }
        free(pixels);
    }
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    // Allocate the t_bmp24 structure
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fprintf(stderr, "Error: failed to allocate t_bmp24 structure.\n");
        return NULL;
    }

    // Initialize image fields
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);
    
    if (!img->data) {
        free(img);
        return NULL;
    }
    return img;
}

// Libération de l'image
void bmp24_free(t_bmp24 *img) {
    if (img) {
        bmp24_freeDataPixels(img->data, img->height);
        free(img);
    }
}

void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


void bmp24_readPixelData(t_bmp24 *img, FILE *file) {
    int offset = img->header.offset;
    fseek(file, offset, SEEK_SET);
    int rowSize = ((img->width * 3 + 3) / 4) * 4;
    int padding = rowSize - (img->width * 3);
    
    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            uint8_t bgr[3];
            fread(bgr, sizeof(uint8_t), 3, file);
            img->data[i][j].blue = bgr[0];
            img->data[i][j].green = bgr[1];
            img->data[i][j].red = bgr[2];
        }
        // Ignorer le padding en fin de ligne
        fseek(file, padding, SEEK_CUR);
    }
}
void bmp24_writePixelData(t_bmp24 *img, FILE *file) {
    int offset = img->header.offset;
    fseek(file, offset, SEEK_SET);
     int rowSize = ((img->width * 3 + 3) / 4) * 4;
    int padding = rowSize - (img->width * 3);
    uint8_t pad[3] = {0, 0, 0}; // max 3 octets de padding
    
    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            uint8_t bgr[3] = {
                img->data[i][j].blue,
                img->data[i][j].green,
                img->data[i][j].red
            };
            fwrite(bgr, sizeof(uint8_t), 3, file);
        }
        // Écrire le padding à la fin de la ligne
        fwrite(pad, sizeof(uint8_t), padding, file);
    }
}

t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s.\n", filename);
        return NULL;
    }

    t_bmp_header header;
    t_bmp_info header_info;

    fread(&header, sizeof(t_bmp_header), 1, file);
    fread(&header_info, sizeof(t_bmp_info), 1, file);

    if (header.type != BMP_TYPE || header_info.bits != 24) {
        fprintf(stderr, "Erreur : le fichier n'est pas une image BMP 24 bits.\n");
        fclose(file);
        return NULL;
    }

    t_bmp24 *img = bmp24_allocate(header_info.width, header_info.height, header_info.bits);
    if (!img) {
        fclose(file);
        return NULL;
    }

    img->header = header;
    img->header_info = header_info;

    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s en écriture.\n", filename);
        return;
    }

    fwrite(&img->header, sizeof(t_bmp_header), 1, file);
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, file);

    bmp24_writePixelData(img, file);
    fclose(file);
}
void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            uint8_t gray = (img->data[i][j].red + img->data[i][j].green + img->data[i][j].blue) / 3;
            img->data[i][j].red = gray;
            img->data[i][j].green = gray;
            img->data[i][j].blue = gray;
        }
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int r = img->data[i][j].red + value;
            int g = img->data[i][j].green + value;
            int b = img->data[i][j].blue + value;

            img->data[i][j].red = (r > 255) ? 255 : (r < 0) ? 0 : r;
            img->data[i][j].green = (g > 255) ? 255 : (g < 0) ? 0 : g;
            img->data[i][j].blue = (b > 255) ? 255 : (b < 0) ? 0 : b;
        }
    }
}

void bmp24_applyConvolution(t_bmp24 *img, float kernel[3][3]) {
    t_pixel **newData = bmp24_allocateDataPixels(img->width, img->height);
    if (!newData) return;

    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            float sumR = 0, sumG = 0, sumB = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    sumR += img->data[py][px].red * kernel[ky + 1][kx + 1];
                    sumG += img->data[py][px].green * kernel[ky + 1][kx + 1];
                    sumB += img->data[py][px].blue * kernel[ky + 1][kx + 1];
                }
            }
            newData[y][x].red = fmin(fmax((int)sumR, 0), 255);
            newData[y][x].green = fmin(fmax((int)sumG, 0), 255);
            newData[y][x].blue = fmin(fmax((int)sumB, 0), 255);
        }
    }

    // Remplacer les pixels
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = newData[y][x];
        }
    }

    bmp24_freeDataPixels(newData, img->height);
}
