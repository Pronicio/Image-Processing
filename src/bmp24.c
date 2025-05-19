#include "bmp24.h"
#include <stdio.h>
#include <stdlib.h>

// Positionner + lire
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

// Positionner + écrire
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

// Allocation matrice pixels
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
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

// Libération matrice
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) free(pixels[i]);
    free(pixels);
}

// Allocation image
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) return NULL;
    img->width      = width;
    img->height     = height;
    img->colorDepth = colorDepth;
    img->data       = bmp24_allocateDataPixels(width, height);
    if (!img->data) { free(img); return NULL; }
    return img;
}

// Libération image
void bmp24_free(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

// Charger une BMP 24 bits
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) { perror("ouvrir BMP"); return NULL; }

    // Lire largeur, hauteur, profondeur
    uint16_t type;
    uint32_t depth;
    file_rawRead(BITMAP_MAGIC, &type, sizeof(type), 1, file);
    file_rawRead(BITMAP_DEPTH, &depth, sizeof(depth), 1, file);

    if (type != BMP_TYPE || depth != DEFAULT_DEPTH) {
        fprintf(stderr, "Format BMP invalide\n");
        fclose(file);
        return NULL;
    }

    // Lire en-têtes complets
    t_bmp_header header;
    t_bmp_info   info;
    file_rawRead(BITMAP_MAGIC,  &header,    sizeof(header), 1, file);
    file_rawRead(BITMAP_OFFSET, &info,      sizeof(info),   1, file);

    // Allouer structure
    t_bmp24 *img = bmp24_allocate(info.width, info.height, info.bits);
    if (!img) { fclose(file); return NULL; }

    img->header      = header;
    img->header_info = info;

    // Lire pixels
    bmp24_readPixelData(file, img);
    fclose(file);
    return img;
}

// Sauvegarder une BMP 24 bits
void bmp24_saveImage(const t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) { perror("écrire BMP"); return; }

    // Écrire en-têtes bruts
    file_rawWrite(BITMAP_MAGIC,  &img->header,      sizeof(img->header),      1, file);
    file_rawWrite(BITMAP_OFFSET, &img->header_info, sizeof(img->header_info), 1, file);

    // Écrire pixels
    bmp24_writePixelData(file, img);
    fclose(file);
}

// Lire toutes les données pixel (BGR, bas-haut)
void bmp24_readPixelData(FILE *file, t_bmp24 *img) {
    const uint32_t start = img->header.offset;
    fseek(file, start, SEEK_SET);
    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            uint8_t bgr[3];
            fread(bgr, 1, 3, file);
            img->data[i][j].blue  = bgr[0];
            img->data[i][j].green = bgr[1];
            img->data[i][j].red   = bgr[2];
        }
    }
}

// Écrire toutes les données pixel (BGR, bas-haut)
void bmp24_writePixelData(FILE *file, const t_bmp24 *img) {
    const uint32_t start = img->header.offset;
    fseek(file, start, SEEK_SET);
    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            uint8_t bgr[3] = {
                img->data[i][j].blue,
                img->data[i][j].green,
                img->data[i][j].red
            };
            fwrite(bgr, 1, 3, file);
        }
    }
}

// Négatif
void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++)
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red   = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue  = 255 - img->data[y][x].blue;
        }
}

// Niveaux de gris (moyenne simple)
void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++)
        for (int x = 0; x < img->width; x++) {
            uint8_t gray = (img->data[y][x].red
                          + img->data[y][x].green
                          + img->data[y][x].blue) / 3;
            img->data[y][x].red =
            img->data[y][x].green =
            img->data[y][x].blue = gray;
        }
}

// Luminosité
void bmp24_brightness(t_bmp24 *img, int value) {
    for (int y = 0; y < img->height; y++)
        for (int x = 0; x < img->width; x++) {
            int r = img->data[y][x].red   + value;
            int g = img->data[y][x].green + value;
            int b = img->data[y][x].blue  + value;
            img->data[y][x].red   = (uint8_t)(r<0?0:(r>255?255:r));
            img->data[y][x].green = (uint8_t)(g<0?0:(g>255?255:g));
            img->data[y][x].blue  = (uint8_t)(b<0?0:(b>255?255:b));
        }
}

// Convolution pour un pixel donné
t_pixel bmp24_convolution(const t_bmp24 *img, int x, int y,
                          float **kernel, int kSize) {
    int half = kSize / 2;
    float sumR=0, sumG=0, sumB=0;
    for (int ky = 0; ky < kSize; ky++) {
        for (int kx = 0; kx < kSize; kx++) {
            int px = x + kx - half;
            int py = y + ky - half;
            // pour ce TP, on ne touche pas aux bords : px,py dans [1..width-2,height-2]
            t_pixel p = img->data[py][px];
            float kval = kernel[ky][kx];
            sumR += p.red   * kval;
            sumG += p.green * kval;
            sumB += p.blue  * kval;
        }
    }
    t_pixel out;
    out.red   = (uint8_t)(sumR<0?0:(sumR>255?255:sumR));
    out.green = (uint8_t)(sumG<0?0:(sumG>255?255:sumG));
    out.blue  = (uint8_t)(sumB<0?0:(sumB>255?255:sumB));
    return out;
}

// Application générique
void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kSize) {
    t_pixel **newData = bmp24_allocateDataPixels(img->width, img->height);
    if (!newData) return;
    for (int y = 1; y < img->height - 1; y++)
        for (int x = 1; x < img->width - 1; x++)
            newData[y][x] = bmp24_convolution(img, x, y, kernel, kSize);
    bmp24_freeDataPixels(img->data, img->height);
    img->data = newData;
}

// Wrappers pour noyaux 3×3
void bmp24_boxBlur(t_bmp24 *img) {
    static float k[3][3] = {
        {1/9.f,1/9.f,1/9.f},
        {1/9.f,1/9.f,1/9.f},
        {1/9.f,1/9.f,1/9.f}
    };
    float *kernel[3] = { k[0], k[1], k[2] };
    bmp24_applyFilter(img, kernel, 3);
}
void bmp24_gaussianBlur(t_bmp24 *img) {
    static float k[3][3] = {
        {1/16.f,2/16.f,1/16.f},
        {2/16.f,4/16.f,2/16.f},
        {1/16.f,2/16.f,1/16.f}
    };
    float *kernel[3] = { k[0], k[1], k[2] };
    bmp24_applyFilter(img, kernel, 3);
}
void bmp24_outline(t_bmp24 *img) {
    static float k[3][3] = {
        {-1,-1,-1},
        {-1, 8,-1},
        {-1,-1,-1}
    };
    float *kernel[3] = { k[0], k[1], k[2] };
    bmp24_applyFilter(img, kernel, 3);
}
void bmp24_emboss(t_bmp24 *img) {
    static float k[3][3] = {
        {-2,-1,0},
        {-1, 1,1},
        { 0, 1,2}
    };
    float *kernel[3] = { k[0], k[1], k[2] };
    bmp24_applyFilter(img, kernel, 3);
}
void bmp24_sharpen(t_bmp24 *img) {
    static float k[3][3] = {
        {0,-1,0},
        {-1,5,-1},
        {0,-1,0}
    };
    float *kernel[3] = { k[0], k[1], k[2] };
    bmp24_applyFilter(img, kernel, 3);
}
