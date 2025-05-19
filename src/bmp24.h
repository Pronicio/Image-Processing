#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Constantes utiles pour l'en-tête BMP
#define BITMAP_MAGIC      0x00  // offset 0
#define BITMAP_SIZE       0x02  // offset 2
#define BITMAP_OFFSET     0x0A  // offset 10 
#define BITMAP_WIDTH      0x12  // offset 18
#define BITMAP_HEIGHT     0x16  // offset 22
#define BITMAP_DEPTH      0x1C  // offset 28
#define BITMAP_SIZE_RAW   0x22  // offset 34
#define BMP_TYPE          0x4D42  // 'BM' en hexadécimal
#define HEADER_SIZE       0x0E  // 14 octets
#define INFO_SIZE         0x28  // 40 octets
#define DEFAULT_DEPTH     0x18  // 24 bits

// En-tête de fichier (14 octets)
typedef struct {
    uint16_t type;        // 'BM' = BMP_TYPE
    uint32_t size;        // taille fichier
    uint16_t reserved1;   // 0
    uint16_t reserved2;   // 0
    uint32_t offset;      // position début des pixels (BITMAP_OFFSET)
} t_bmp_header;

// Info header (BITMAPINFOHEADER, 40 octets)
typedef struct {
    uint32_t size;           // HEADER_SIZE + INFO_SIZE
    int32_t  width;          // largeur
    int32_t  height;         // hauteur
    uint16_t planes;         // =1
    uint16_t bits;           // profondeur (24)
    uint32_t compression;    // 0 = pas de compression
    uint32_t imagesize;      // taille données image
    int32_t  xresolution;    // résolution X
    int32_t  yresolution;    // résolution Y
    uint32_t ncolors;        // couleurs utilisées (0 si toutes)
    uint32_t importantcolors;// importantes (0 = toutes)
} t_bmp_info;

// Un pixel couleur (BGR dans le fichier, RGB en mémoire)
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;  

// Structure d’image 24 bits
typedef struct {
    t_bmp_header  header;
    t_bmp_info    header_info;
    int           width;
    int           height;
    int           colorDepth;  // doit valoir 24
    t_pixel     **data;        // matrice [height][width]
} t_bmp24;

// Fonctions de lecture/écriture brute
void file_rawRead (uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);
void file_rawWrite(uint32_t position, const void *buffer, uint32_t size, size_t n, FILE *file);

// Allocation / libération
t_pixel **bmp24_allocateDataPixels(int width, int height);
void      bmp24_freeDataPixels   (t_pixel **pixels, int height);
t_bmp24  *bmp24_allocate         (int width, int height, int colorDepth);
void      bmp24_free             (t_bmp24 *img);

// I/O 24 bits
t_bmp24  *bmp24_loadImage(const char *filename);
void      bmp24_saveImage(const t_bmp24 *img, const char *filename);
void bmp24_printInfo(const t_bmp24 *img);


// Lecture/écriture des pixels
void bmp24_readPixelData(FILE *file, t_bmp24 *img);
void bmp24_writePixelData(FILE *file, const t_bmp24 *img);

// Traitements de base
void bmp24_negative   (t_bmp24 *img);
void bmp24_grayscale  (t_bmp24 *img);
void bmp24_brightness (t_bmp24 *img, int value);

// Convolution et filtres
t_pixel bmp24_convolution(const t_bmp24 *img, int x, int y, float **kernel, int kernelSize);
void    bmp24_applyFilter (t_bmp24 *img, float **kernel, int kernelSize);
void    bmp24_boxBlur     (t_bmp24 *img);
void    bmp24_gaussianBlur(t_bmp24 *img);
void    bmp24_outline     (t_bmp24 *img);
void    bmp24_emboss      (t_bmp24 *img);
void    bmp24_sharpen     (t_bmp24 *img);

#endif // BMP24_H

