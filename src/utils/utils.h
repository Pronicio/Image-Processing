#ifndef UTILS_H
#define UTILS_H

typedef enum {
    BMP_UNKNOWN = 0,
    BMP_8BIT = 8,
    BMP_24BIT = 24,
    BMP_ERROR = -1
} BMP_Type;

/**
 * Détermine si un fichier BMP est au format 8 bits ou 24 bits
 *
 * @param filename Le chemin vers le fichier BMP
 * @return BMP_Type: BMP_8BIT (8), BMP_24BIT (24), BMP_UNKNOWN (0) ou BMP_ERROR (-1) en cas d'erreur
 */
BMP_Type bmp_getFileType(const char *filename);


#endif
