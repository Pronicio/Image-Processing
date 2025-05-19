#include "bmp24.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Helper: calculate row padding (each row multiple of 4 bytes)
static int calculatePadding(int width) {
    return (4 - (width * 3) % 4) % 4;
}

// Load BMP image
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { perror("Error opening file"); return NULL; }
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) { fclose(fp); return NULL; }
    fread(&img->fileHeader, sizeof(t_bmp_header), 1, fp);
    fread(&img->infoHeader, sizeof(t_bmp_info), 1, fp);
    img->width   = img->infoHeader.width;
    img->height  = img->infoHeader.height < 0
                   ? -img->infoHeader.height
                   : img->infoHeader.height;
    img->padding = calculatePadding(img->width);
    img->data    = bmp24_allocateDataPixels(img->width, img->height);
    if (!img->data) { free(img); fclose(fp); return NULL; }
    fseek(fp, img->fileHeader.offset, SEEK_SET);
    bmp24_readPixelData(fp, img);
    fclose(fp);
    return img;
}

// Save BMP image
int bmp24_saveImage(const char *filename, t_bmp24 *img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) { perror("Error opening file"); return -1; }
    fwrite(&img->fileHeader, sizeof(t_bmp_header), 1, fp);
    fwrite(&img->infoHeader, sizeof(t_bmp_info), 1, fp);
    bmp24_writePixelData(fp, img);
    fclose(fp);
    return 0;
}

// Free image
void bmp24_freeImage(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

// Print info
void bmp24_printInfo(const t_bmp24 *img) {
    printf("File size: %u bytes\n", img->fileHeader.size);
    printf("Image offset: %u bytes\n", img->fileHeader.offset);
    printf("Header size: %u bytes\n", img->infoHeader.headerSize);
    printf("Dimensions: %d x %d pixels\n", img->width, img->height);
    printf("Bits per pixel: %u\n", img->infoHeader.bitsPerPixel);
    printf("Image data size: %u bytes\n", img->infoHeader.imageSize);
}

// Allocate pixel data
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel*));
    if (!pixels) return NULL;
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            while (i--) free(pixels[i]);
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

// Free pixel data
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) free(pixels[i]);
    free(pixels);
}

// Read pixels with padding
void bmp24_readPixelData(FILE *fp, t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fread(&img->data[img->height - 1 - y][x],
                  sizeof(t_pixel), 1, fp);
        }
        fseek(fp, img->padding, SEEK_CUR);
    }
}

// Write pixels with padding
void bmp24_writePixelData(FILE *fp, const t_bmp24 *img) {
    uint8_t pad[3] = {0};
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fwrite(&img->data[img->height - 1 - y][x],
                   sizeof(t_pixel), 1, fp);
        }
        fwrite(pad, img->padding, 1, fp);
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

    if (header.type != BMP_TYPE) {
        fprintf(stderr, "Erreur : signature BMP invalide (0x%04X).\n", header.type);
        fclose(file);
        return NULL;
    }
    if (header_info.biBitCount != 24) {
        fprintf(stderr,
            "Erreur : profondeur de couleur incorrecte (%u bits), attendu 24 bits.\n",
            header_info.biBitCount);
        fclose(file);
        return NULL;
    }
    
    t_bmp24 *img = bmp24_allocate(
        header_info.biWidth,
        header_info.biHeight,
        header_info.biBitCount
    );
    
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
static void applyFilter24(t_bmp24 *img, float kernel[3][3]) {
       // Déléguer entièrement à bmp24_applyConvolution
        bmp24_applyConvolution(img, kernel);
}

void bmp24_boxBlur(t_bmp24 *img) {
    float k[3][3] = {
        {1/9.0f,1/9.0f,1/9.0f},
        {1/9.0f,1/9.0f,1/9.0f},
        {1/9.0f,1/9.0f,1/9.0f}
    };
    applyFilter24(img, k);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    float k[3][3] = {
        {1/16.0f,2/16.0f,1/16.0f},
        {2/16.0f,4/16.0f,2/16.0f},
        {1/16.0f,2/16.0f,1/16.0f}
    };
    applyFilter24(img, k);
}

void bmp24_outline(t_bmp24 *img) {
    float k[3][3] = {
        {-1,-1,-1},
        {-1, 8,-1},
        {-1,-1,-1}
    };
    applyFilter24(img, k);
}

void bmp24_emboss(t_bmp24 *img) {
    float k[3][3] = {
        {-2,-1,0},
        {-1, 1,1},
        { 0, 1,2}
    };
    applyFilter24(img, k);
}

void bmp24_sharpen(t_bmp24 *img) {
    float k[3][3] = {
        { 0,-1, 0},
        {-1, 5,-1},
        { 0,-1, 0}
    };
    applyFilter24(img, k);
}

void bmp24_printInfo(t_bmp24 *img) {
    printf("Image24 Info:\n");
    printf(" Width: %d\n", img->width);
    printf(" Height: %d\n", img->height);
    printf(" Color Depth: %d\n", img->colorDepth);
    // taille data = width*height*3
    printf(" Data Size: %d\n", img->width * img->height * 3);
}
