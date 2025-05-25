#include "color.h"

#include <stdlib.h>
#include <string.h>

/**
 * Positionne le curseur de fichier à la position dans le fichier file,
 * puis lit n éléments de taille size dans buffer
 *
 * @param position La position à partir de laquelle il faut lire dans file
 * @param buffer Le buffer pour stocker les données lues
 * @param size La taille de chaque élément à lire
 * @param n Le nombre d'éléments à lire
 * @param file Le descripteur de fichier dans lequel il faut lire
 */
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

/**
 * Positionne le curseur de fichier à la position dans le fichier file,
 * puis écrit n éléments de taille size depuis le buffer
 *
 * @param position La position à partir de laquelle il faut écrire dans file
 * @param buffer Le buffer contenant les éléments à écrire
 * @param size La taille de chaque élément à écrire
 * @param n Le nombre d'éléments à écrire
 * @param file Le descripteur de fichier dans lequel il faut écrire
 */
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

/**
 * Alloue de la mémoire pour les pixels d'une image BMP 24 bits
 *
 * @param width Largeur de l'image en pixels
 * @param height Hauteur de l'image en pixels
 * @return t_pixel**: Tableau bidimensionnel de pixels ou NULL en cas d'erreur
 */
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        fprintf(stderr, "Erreur: Échec de l'allocation mémoire pour les lignes de pixels\n");
        return NULL;
    }

    // Initialiser tous les pointeurs à NULL pour faciliter la libération en cas d'erreur
    for (int i = 0; i < height; i++) {
        pixels[i] = NULL;
    }

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {
            fprintf(stderr, "Erreur: Échec de l'allocation mémoire pour la ligne %d\n", i);
            // Libérer la mémoire déjà allouée
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}

/**
 * Libère la mémoire allouée pour les pixels d'une image BMP 24 bits
 *
 * @param pixels Tableau de pixels à libérer
 * @param height Hauteur de l'image en pixels
 */
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels == NULL) return;

    for (int i = 0; i < height; i++) {
        if (pixels[i] != NULL) {
            free(pixels[i]);
        }
    }
    free(pixels);
}

/**
 * Alloue de la mémoire pour une structure d'image BMP 24 bits
 *
 * @param width Largeur de l'image en pixels
 * @param height Hauteur de l'image en pixels
 * @param colorDepth Profondeur de couleur en bits (devrait être 24)
 * @return t_bmp24*: Pointeur vers l'image créée ou NULL en cas d'erreur
 */
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fprintf(stderr, "Erreur: Échec de l'allocation mémoire pour l'image\n");
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    img->data = bmp24_allocateDataPixels(width, height);
    if (img->data == NULL) {
        fprintf(stderr, "Erreur: Échec de l'allocation mémoire pour les données de l'image\n");
        free(img);
        return NULL;
    }

    return img;
}

/**
 * Libère la mémoire allouée pour une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à libérer
 */
void bmp24_free(t_bmp24 *img) {
    if (img == NULL) return;

    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

/**
 * Lit la valeur d'un pixel à une position spécifique
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param file Fichier BMP ouvert
 */
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    int y_pos = image->height - 1 - y; // Inverser les lignes (bas en haut)
    fseek(file, image->header.offset + (y_pos * image->width + x) * 3, SEEK_SET);
    fread(&image->data[y][x], sizeof(t_pixel), 1, file);
}

/**
 * Lit toutes les données de pixels d'une image BMP 24 bits
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param file Fichier BMP ouvert
 */
void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}

/**
 * Écrit la valeur d'un pixel à une position spécifique
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param file Fichier BMP ouvert
 */
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    fwrite(&image->data[y][x], sizeof(t_pixel), 1, file);
}

/**
 * Écrit toutes les données de pixels d'une image BMP 24 bits
 *
 * @param image Pointeur vers l'image BMP 24 bits
 * @param file Fichier BMP ouvert
 */
void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}

/**
 * Charge une image BMP 24 bits à partir d'un fichier
 *
 * @param filename Nom du fichier à charger
 * @return t_bmp24*: Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
t_bmp24 *bmp24_loadImage(const char *filename) {
    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    // Ouvrir le fichier en mode binaire lecture
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Lire l'en-tête du fichier BMP
    t_bmp_header header;
    fread(&header, sizeof(t_bmp_header), 1, file);

    // Vérifier que c'est bien un fichier BMP valide
    if (header.type != BMP_TYPE) {
        fprintf(stderr, "Erreur: Le fichier %s n'est pas un fichier BMP valide\n", filename);
        fclose(file);
        return NULL;
    }

    // Lire les informations d'en-tête de l'image
    t_bmp_info header_info;
    fread(&header_info, sizeof(t_bmp_info), 1, file);

    // Vérifier que c'est une image 24 bits
    if (header_info.bits != 24) {
        fprintf(stderr, "Erreur: Le fichier %s n'est pas une image 24 bits\n", filename);
        fclose(file);
        return NULL;
    }

    // Vérifier les dimensions (limiter à une taille raisonnable)
    if (header_info.width <= 0 || header_info.width > 10000 ||
        header_info.height <= 0 || header_info.height > 10000) {
        fprintf(stderr, "Erreur: Dimensions d'image invalides (%d x %d)\n",
                header_info.width, header_info.height);
        fclose(file);
        return NULL;
        }

    // Allouer une structure t_bmp24 avec les dimensions lues
    t_bmp24 *image = bmp24_allocate(header_info.width, header_info.height, 24);
    if (image == NULL) {
        fprintf(stderr, "Erreur: Impossible d'allouer la mémoire pour l'image\n");
        fclose(file);
        return NULL;
    }

    // Copier les en-têtes lus dans la structure
    image->header = header;
    image->header_info = header_info;

    // Lire les données des pixels
    bmp24_readPixelData(image, file);

    // Fermer le fichier
    fclose(file);

    return image;
}

/**
 * Sauvegarde une image BMP 24 bits dans un fichier
 *
 * @param img Pointeur vers l'image à sauvegarder
 * @param filename Nom du fichier de destination
 */
void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    // Ouvrir le fichier en mode binaire écriture
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s en écriture\n", filename);
        return;
    }

    // Écrire l'en-tête du fichier BMP
    file_rawWrite(BITMAP_MAGIC, &img->header, HEADER_SIZE, 1, file);

    // Écrire les informations d'en-tête de l'image
    file_rawWrite(HEADER_SIZE, &img->header_info, INFO_SIZE, 1, file);

    // Calculer la taille de l'image
    file_rawWrite(img->header.offset, &img->header_info, img->header_info.imagesize, 1, file);

    // Écrire les données des pixels
    bmp24_writePixelData(img, file);

    // Fermer le fichier
    fclose(file);
}

/**
 * Affiche les informations d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image dont on veut afficher les informations
 */
void bmp24_printInfo(t_bmp24 *img) {
    if (img == NULL) {
        printf("⚠️ Image non valide ou non chargée\n");
        return;
    }

    printf("📏 Résolution: %d x %d pixels\n", img->width, img->height);
    printf("🎨 Profondeur de couleur: %d bits\n", img->colorDepth);
    printf("📊 Taille du fichier: %u octets\n", img->header.size);
    printf("📍 Offset des données: %u octets\n", img->header.offset);
    printf("🖼️ Taille des données image: %u octets\n", img->header_info.imagesize);

    // Informations supplémentaires sur l'en-tête
    printf("🔄 Compression: %u\n", img->header_info.compression);
    printf("📐 Résolution X: %d pixels/mètre\n", img->header_info.xresolution);
    printf("📐 Résolution Y: %d pixels/mètre\n", img->header_info.yresolution);

    if (img->header_info.ncolors > 0) {
        printf("🎭 Nombre de couleurs: %u\n", img->header_info.ncolors);
    }
}

/**
 * Applique un effet négatif à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

/**
 * Convertit une image BMP 24 bits en niveaux de gris
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Calculer la valeur moyenne des 3 canaux de couleur
            unsigned char moyenne = (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3;

            // Affecter cette valeur moyenne à chaque canal
            img->data[y][x].red = moyenne;
            img->data[y][x].green = moyenne;
            img->data[y][x].blue = moyenne;
        }
    }
}

/**
 * Modifie la luminosité d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 * @param value Valeur de luminosité à ajouter (-255 à 255)
 */
void bmp24_brightness(t_bmp24 *img, int value) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Ajuster le canal rouge
            int new_red = img->data[y][x].red + value;
            if (new_red > 255) {
                new_red = 255;
            } else if (new_red < 0) {
                new_red = 0;
            }
            img->data[y][x].red = (uint8_t)new_red;

            // Ajuster le canal vert
            int new_green = img->data[y][x].green + value;
            if (new_green > 255) {
                new_green = 255;
            } else if (new_green < 0) {
                new_green = 0;
            }
            img->data[y][x].green = (uint8_t)new_green;

            // Ajuster le canal bleu
            int new_blue = img->data[y][x].blue + value;
            if (new_blue > 255) {
                new_blue = 255;
            } else if (new_blue < 0) {
                new_blue = 0;
            }
            img->data[y][x].blue = (uint8_t)new_blue;
        }
    }
}

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
t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    // Calculer la moitié de la taille du noyau
    int n = kernelSize / 2;

    // Initialiser les sommes pour chaque canal de couleur
    float sumRed = 0.0f, sumGreen = 0.0f, sumBlue = 0.0f;

    // Appliquer le noyau de convolution
    for (int i = -n; i <= n; i++) {
        for (int j = -n; j <= n; j++) {
            // Calculer les coordonnées du pixel voisin
            int neighborX = x + j;
            int neighborY = y + i;

            // Gérer les bords de l'image (clamp)
            if (neighborX < 0) neighborX = 0;
            if (neighborX >= img->width) neighborX = img->width - 1;
            if (neighborY < 0) neighborY = 0;
            if (neighborY >= img->height) neighborY = img->height - 1;

            // Calculer l'indice dans le noyau
            int kernelI = i + n;
            int kernelJ = j + n;

            // Appliquer la valeur du noyau à chaque canal
            sumRed += img->data[neighborY][neighborX].red * kernel[kernelI][kernelJ];
            sumGreen += img->data[neighborY][neighborX].green * kernel[kernelI][kernelJ];
            sumBlue += img->data[neighborY][neighborX].blue * kernel[kernelI][kernelJ];
        }
    }

    // Créer le nouveau pixel avec les valeurs calculées
    t_pixel result;
    result.red = (sumRed > 255) ? 255 : ((sumRed < 0) ? 0 : (uint8_t)sumRed);
    result.green = (sumGreen > 255) ? 255 : ((sumGreen < 0) ? 0 : (uint8_t)sumGreen);
    result.blue = (sumBlue > 255) ? 255 : ((sumBlue < 0) ? 0 : (uint8_t)sumBlue);

    return result;
}

/**
 * Applique un flou rectangulaire à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_boxBlur(t_bmp24 *img) {
    // Noyau de flou uniforme 3x3
    int kernelSize = 3;
    float **kernel = malloc(kernelSize * sizeof(float *));
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = malloc(kernelSize * sizeof(float));
        for (int j = 0; j < kernelSize; j++) {
            kernel[i][j] = 1.0f / 9.0f;  // Chaque élément vaut 1/9
        }
    }

    // Créer une copie de l'image pour éviter de modifier l'original pendant le traitement
    t_bmp24 *imgCopy = malloc(sizeof(t_bmp24));
    imgCopy->width = img->width;
    imgCopy->height = img->height;
    imgCopy->data = malloc(img->height * sizeof(t_pixel *));
    for (int y = 0; y < img->height; y++) {
        imgCopy->data[y] = malloc(img->width * sizeof(t_pixel));
        memcpy(imgCopy->data[y], img->data[y], img->width * sizeof(t_pixel));
    }

    // Appliquer le filtre à chaque pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(imgCopy, x, y, kernel, kernelSize);
        }
    }

    // Libérer la mémoire
    for (int y = 0; y < imgCopy->height; y++) {
        free(imgCopy->data[y]);
    }
    free(imgCopy->data);
    free(imgCopy);

    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Applique un flou gaussien à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_gaussianBlur(t_bmp24 *img) {
    // Noyau gaussien 5x5
    int kernelSize = 5;
    float **kernel = malloc(kernelSize * sizeof(float *));
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = malloc(kernelSize * sizeof(float));
    }

    // Coefficients du noyau gaussien 5x5
    kernel[0][0] = 1/256.0f; kernel[0][1] = 4/256.0f;  kernel[0][2] = 6/256.0f;  kernel[0][3] = 4/256.0f;  kernel[0][4] = 1/256.0f;
    kernel[1][0] = 4/256.0f; kernel[1][1] = 16/256.0f; kernel[1][2] = 24/256.0f; kernel[1][3] = 16/256.0f; kernel[1][4] = 4/256.0f;
    kernel[2][0] = 6/256.0f; kernel[2][1] = 24/256.0f; kernel[2][2] = 36/256.0f; kernel[2][3] = 24/256.0f; kernel[2][4] = 6/256.0f;
    kernel[3][0] = 4/256.0f; kernel[3][1] = 16/256.0f; kernel[3][2] = 24/256.0f; kernel[3][3] = 16/256.0f; kernel[3][4] = 4/256.0f;
    kernel[4][0] = 1/256.0f; kernel[4][1] = 4/256.0f;  kernel[4][2] = 6/256.0f;  kernel[4][3] = 4/256.0f;  kernel[4][4] = 1/256.0f;

    // Créer une copie de l'image
    t_bmp24 *imgCopy = malloc(sizeof(t_bmp24));
    imgCopy->width = img->width;
    imgCopy->height = img->height;
    imgCopy->data = malloc(img->height * sizeof(t_pixel *));
    for (int y = 0; y < img->height; y++) {
        imgCopy->data[y] = malloc(img->width * sizeof(t_pixel));
        memcpy(imgCopy->data[y], img->data[y], img->width * sizeof(t_pixel));
    }

    // Appliquer le filtre à chaque pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(imgCopy, x, y, kernel, kernelSize);
        }
    }

    // Libérer la mémoire
    for (int y = 0; y < imgCopy->height; y++) {
        free(imgCopy->data[y]);
    }
    free(imgCopy->data);
    free(imgCopy);

    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Détecte les contours d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_outline(t_bmp24 *img) {
    // Noyau de détection de contours (Laplacien)
    int kernelSize = 3;
    float **kernel = malloc(kernelSize * sizeof(float *));
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = malloc(kernelSize * sizeof(float));
    }

    // Coefficients du noyau de détection de contours
    kernel[0][0] = -1.0f; kernel[0][1] = -1.0f; kernel[0][2] = -1.0f;
    kernel[1][0] = -1.0f; kernel[1][1] = 8.0f;  kernel[1][2] = -1.0f;
    kernel[2][0] = -1.0f; kernel[2][1] = -1.0f; kernel[2][2] = -1.0f;

    // Créer une copie de l'image
    t_bmp24 *imgCopy = malloc(sizeof(t_bmp24));
    imgCopy->width = img->width;
    imgCopy->height = img->height;
    imgCopy->data = malloc(img->height * sizeof(t_pixel *));
    for (int y = 0; y < img->height; y++) {
        imgCopy->data[y] = malloc(img->width * sizeof(t_pixel));
        memcpy(imgCopy->data[y], img->data[y], img->width * sizeof(t_pixel));
    }

    // Appliquer le filtre à chaque pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(imgCopy, x, y, kernel, kernelSize);
        }
    }

    // Libérer la mémoire
    for (int y = 0; y < imgCopy->height; y++) {
        free(imgCopy->data[y]);
    }
    free(imgCopy->data);
    free(imgCopy);

    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Applique un effet de relief à une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_emboss(t_bmp24 *img) {
    // Noyau de relief
    int kernelSize = 3;
    float **kernel = malloc(kernelSize * sizeof(float *));
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = malloc(kernelSize * sizeof(float));
    }

    // Coefficients du noyau de relief
    kernel[0][0] = -2.0f; kernel[0][1] = -1.0f; kernel[0][2] = 0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] = 1.0f;  kernel[1][2] = 1.0f;
    kernel[2][0] = 0.0f;  kernel[2][1] = 1.0f;  kernel[2][2] = 2.0f;

    // Créer une copie de l'image
    t_bmp24 *imgCopy = malloc(sizeof(t_bmp24));
    imgCopy->width = img->width;
    imgCopy->height = img->height;
    imgCopy->data = malloc(img->height * sizeof(t_pixel *));
    for (int y = 0; y < img->height; y++) {
        imgCopy->data[y] = malloc(img->width * sizeof(t_pixel));
        memcpy(imgCopy->data[y], img->data[y], img->width * sizeof(t_pixel));
    }

    // Appliquer le filtre à chaque pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(imgCopy, x, y, kernel, kernelSize);
        }
    }

    // Libérer la mémoire
    for (int y = 0; y < imgCopy->height; y++) {
        free(imgCopy->data[y]);
    }
    free(imgCopy->data);
    free(imgCopy);

    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Améliore la netteté d'une image BMP 24 bits
 *
 * @param img Pointeur vers l'image à modifier
 */
void bmp24_sharpen(t_bmp24 *img) {
    // Noyau de netteté
    int kernelSize = 3;
    float **kernel = malloc(kernelSize * sizeof(float *));
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = malloc(kernelSize * sizeof(float));
    }

    // Coefficients du noyau d'amélioration de la netteté
    kernel[0][0] = 0.0f;  kernel[0][1] = -1.0f; kernel[0][2] = 0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] = 5.0f;  kernel[1][2] = -1.0f;
    kernel[2][0] = 0.0f;  kernel[2][1] = -1.0f; kernel[2][2] = 0.0f;

    // Créer une copie de l'image
    t_bmp24 *imgCopy = malloc(sizeof(t_bmp24));
    imgCopy->width = img->width;
    imgCopy->height = img->height;
    imgCopy->data = malloc(img->height * sizeof(t_pixel *));
    for (int y = 0; y < img->height; y++) {
        imgCopy->data[y] = malloc(img->width * sizeof(t_pixel));
        memcpy(imgCopy->data[y], img->data[y], img->width * sizeof(t_pixel));
    }

    // Appliquer le filtre à chaque pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = bmp24_convolution(imgCopy, x, y, kernel, kernelSize);
        }
    }

    // Libérer la mémoire
    for (int y = 0; y < imgCopy->height; y++) {
        free(imgCopy->data[y]);
    }
    free(imgCopy->data);
    free(imgCopy);

    for (int i = 0; i < kernelSize; i++) {
        free(kernel[i]);
    }
    free(kernel);
}
