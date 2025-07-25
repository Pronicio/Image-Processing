#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <stdio.h>

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34

// Constante pour le type de fichier BMP
#define BMP_TYPE 0x4D42 // 'BM' en hexadécimal

// Constantes pour les tailles des champs de l'en-tête BMP
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets

// Constantes pour les valeurs de profondeur de couleur
#define DEFAULT_DEPTH 0x18 // 24

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

// Force l'alignement sur 1 octet
// Pour éviter les problèmes de compatibilité entre différentes architectures
#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;
#pragma pack(pop)

typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;

/**
 * Alloue de la mémoire pour les pixels d'une image BMP 24 bits
 *
 * @param width Largeur de l'image en pixels
 * @param height Hauteur de l'image en pixels
 * @return t_pixel**: Tableau bidimensionnel de pixels ou NULL en cas d'erreur
 */
t_pixel **bmp24_allocateDataPixels(int width, int height);

/**
 * Libère la mémoire allouée pour les pixels d'une image BMP 24 bits
 *
 * @param pixels Tableau de pixels à libérer
 * @param height Hauteur de l'image en pixels
 */
void bmp24_freeDataPixels(t_pixel **pixels, int height);

/**
 * Alloue de la mémoire pour une structure d'image BMP 24 bits
 *
 * @param width Largeur de l'image en pixels
 * @param height Hauteur de l'image en pixels
 * @param colorDepth Profondeur de couleur en bits (devrait être 24)
 * @return t_bmp24*: Pointeur vers l'image créée ou NULL en cas d'erreur
 */
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);

/**
 * Libère la mémoire allouée pour une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à libérer
 */
void bmp24_free(t_bmp24 *img);

/**
 * Lit la valeur d'un pixel à une position spécifique
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param file Fichier BMP ouvert
 */
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file);

/**
 * Lit toutes les données de pixels d'une image BMP 24 bits
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param file Fichier BMP ouvert
 */
void bmp24_readPixelData(t_bmp24 *image, FILE *file);

/**
 * Écrit la valeur d'un pixel à une position spécifique
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param file Fichier BMP ouvert
 */
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file);

/**
 * Écrit toutes les données de pixels d'une image BMP 24 bits
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param file Fichier BMP ouvert
 */
void bmp24_writePixelData(t_bmp24 *image, FILE *file);

/**
 * Charge une image BMP 24 bits à partir d'un fichier
 *
 * @param filename Nom du fichier à charger
 * @return t_bmp24*: Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
t_bmp24 * bmp24_loadImage(const char * filename);

/**
 * Sauvegarde une image BMP 24 bits dans un fichier
 *
 * @param img Pointeur vers l'image à sauvegarder
 * @param filename Nom du fichier de destination
 */
void bmp24_saveImage(t_bmp24 * img, const char * filename);

/**
 * Affiche les informations d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image dont on veut afficher les informations
 */
void bmp24_printInfo(t_bmp24 *img);

/**
 * Applique un effet négatif à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_negative(t_bmp24 * img);

/**
 * Convertit une image BMP 24 bits en niveaux de gris
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_grayscale(t_bmp24 * img);

/**
 * Modifie la luminosité d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 * @param value Valeur de luminosité à ajouter (-255 à 255)
 */
void bmp24_brightness(t_bmp24 * img, int value);

/**
 * Applique un noyau de convolution à un pixel d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image source
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param kernel Noyau de convolution à appliquer
 * @param kernelSize Taille du noyau (doit être impair)
 * @return t_pixel: Nouvelle valeur du pixel après convolution
 */
t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize);

/**
 * Applique un flou rectangulaire à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_boxBlur(t_bmp24 *img);

/**
 * Applique un flou gaussien à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_gaussianBlur(t_bmp24 *img);

/**
 * Détecte les contours d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_outline(t_bmp24 *img);

/**
 * Applique un effet de relief à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_emboss(t_bmp24 *img);

/**
 * Améliore la netteté d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_sharpen(t_bmp24 *img);

#endif //COLOR_H
