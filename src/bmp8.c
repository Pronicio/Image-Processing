#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Charge une image BMP 8 bits à partir d'un fichier
 *
 * @param filename Le chemin vers le fichier à charger
 * @return t_bmp8*: Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
t_bmp8 *bmp8_loadImage(const char *filename) {
    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    FILE *file = fopen(path, "rb"); // Ouverture en mode lecture binaire

    if (file == NULL) {
        fprintf(stderr, "Fichier non trouvé\n");
        return NULL;
    }

    // Allocation de mémoire pour l'image
    t_bmp8 *img = (t_bmp8 *) malloc(sizeof(t_bmp8));
    if (img == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête (54 octets)
    if (fread(img->header, 1, 54, file) != 54) {
        fprintf(stderr, "Erreur lors de la lecture de l'en-tête\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Vérification s'il s'agit d'un fichier BMP (l'en-tête doit commencer par 'BM')
    if (img->header[0] != 'B' || img->header[1] != 'M') {
        fprintf(stderr, "Fichier BMP non valide\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Extraction des informations de l'image depuis l'en-tête
    // Source : TABLE 1 - Structure d'en-tête d'image BMP
    img->width = *(unsigned int *) &img->header[18];
    img->height = *(unsigned int *) &img->header[22];
    img->colorDepth = *(unsigned short *) &img->header[28];
    img->dataSize = *(unsigned int *) &img->header[34];

    // Vérification s'il s'agit d'une image 8 bits
    if (img->colorDepth != 8) {
        fprintf(stderr, "Ce n'est pas une image BMP 8 bits (profondeur de couleur : %d bits)\n", img->colorDepth);
        free(img);
        fclose(file);
        return NULL;
    }

    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Erreur lors de la lecture de la table de couleurs\n");
        free(img->colorTable);
        free(img->header);
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture des données de l'image
    img->data = (unsigned char *) malloc(img->dataSize * sizeof(unsigned char));
    if (img->data == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les données de l'image\n");
        free(img);
        fclose(file);
        return NULL;
    }

    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Erreur lors de la lecture des données de l'image\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    // Fermeture du fichier
    if (fclose(file) == EOF) {
        // EOF est une macro qui représente la fin d'un fichier
        fprintf(stderr, "Impossible de fermer le fichier\n");
        free(img->data);
        free(img);
        return NULL;
    }

    return img;
}

/**
 * Sauvegarde une image BMP 8 bits dans un fichier
 *
 * @param img L'image à sauvegarder
 * @param filename Le nom du fichier de destination
 */
void bmp8_saveImage(t_bmp8 *img, const char *filename) {
    if (img == NULL) {
        fprintf(stderr, "Impossible de sauvegarder une image NULL\n");
        return;
    }

    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    printf("%s \n\n", path);

    FILE *file = fopen(path, "wb"); // Ouverture en mode écriture binaire

    if (file == NULL) {
        fprintf(stderr, "Impossible de créer le fichier : %s\n", filename);
        return;
    }

    fwrite(img->header, 1, 54, file);
    fwrite(img->colorTable, 1, 1024, file);
    fwrite(img->data, 1, img->dataSize, file);
    fclose(file);
}

/**
 * Libère la mémoire allouée pour une image BMP 8 bits
 *
 * @param img L'image à libérer
 */
void bmp8_free(t_bmp8 *img) {
    if (img == NULL) { return; }
    free(img->data);
    free(img);
}

/**
 * Affiche les informations d'une image BMP 8 bits
 *
 * @param img L'image dont on veut afficher les informations
 */
void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Impossible d'afficher les informations d'une image NULL\n");
        return;
    }

    printf("📐 Largeur : %u\n", img->width);
    printf("📐 Hauteur : %u\n", img->height);
    printf("🎨 Profondeur de couleur : %u\n", img->colorDepth);
    printf("🖼️ Taille des données : %u\n", img->dataSize);
}

/**
 * Applique un effet négatif à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Impossible d'appliquer l'effet négatif à une image NULL\n");
        return;
    }

    // Parcours de chaque pixel de l'image
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Inversion de la valeur du pixel (255 - valeur)
        img->data[i] = 255 - img->data[i];
    }
}

/**
 * Modifie la luminosité d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 * @param value La valeur à ajouter à chaque pixel (-255 à 255)
 */
void bmp8_brightness(const t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Impossible d'ajuster la luminosité d'une image NULL\n");
        return;
    }

    // Parcours de chaque pixel de l'image
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Calcul de la nouvelle valeur du pixel
        int newPixelValue = img->data[i] + value;

        // Vérification que la valeur reste dans l'intervalle valide [0, 255]
        if (newPixelValue > 255) {
            newPixelValue = 255;
        } else if (newPixelValue < 0) {
            newPixelValue = 0;
        }

        // Attribution de la nouvelle valeur au pixel
        img->data[i] = (unsigned char) newPixelValue;
    }
}

/**
 * Applique un seuillage à une image BMP 8 bits
 *
 * @param img L'image à modifier
 * @param threshold La valeur de seuil (0-255)
 */
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Impossible d'appliquer le seuillage à une image NULL\n");
        return;
    }

    // Parcours de chaque pixel de l'image
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Application du seuil
        if (img->data[i] >= threshold) {
            img->data[i] = 255; // Blanc si valeur >= seuil
        } else {
            img->data[i] = 0; // Noir si valeur < seuil
        }
    }
}

/**
 * Applique un filtre à une image BMP 8 bits en utilisant un noyau de convolution
 *
 * @param img L'image à modifier
 * @param kernel Le noyau de convolution à appliquer
 * @param kernelSize La taille du noyau (doit être impair)
 */
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL || kernel == NULL) {
        fprintf(stderr, "Impossible d'appliquer un filtre à une image NULL\n");
        return;
    }

    // Calculer la moitié de la taille du noyau
    int n = kernelSize / 2;

    // Créer une copie des données de l'image pour éviter de modifier les valeurs pendant le calcul
    unsigned char *tempData = (unsigned char *) malloc(img->dataSize * sizeof(unsigned char));
    if (tempData == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la copie temporaire\n");
        return;
    }
    memcpy(tempData, img->data, img->dataSize);

    // Appliquer le filtre seulement sur les pixels internes (éviter les bords)
    for (unsigned int y = n; y < img->height - n; y++) {
        for (unsigned int x = n; x < img->width - n; x++) {
            float sum = 0.0f;

            // Appliquer la convolution
            for (int i = -n; i <= n; i++) {
                for (int j = -n; j <= n; j++) {
                    // Calculer l'index du pixel voisin
                    unsigned int neighborIdx = (y + i) * img->width + (x + j);
                    // Calculer l'index dans le noyau (i+n, j+n pour convertir de [-n,n] à [0,kernelSize-1])
                    int kernelIdx_i = i + n;
                    int kernelIdx_j = j + n;

                    sum += tempData[neighborIdx] * kernel[kernelIdx_i][kernelIdx_j];
                }
            }

            // Limiter la valeur calculée à l'intervalle [0, 255]
            int newValue = (int) sum;
            if (newValue > 255) newValue = 255;
            if (newValue < 0) newValue = 0;

            // Mettre à jour le pixel dans l'image originale
            img->data[y * img->width + x] = (unsigned char) newValue;
        }
    }

    // Libérer la mémoire de la copie temporaire
    free(tempData);
}

/**
 * Applique un flou rectangulaire à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_box_blur(t_bmp8 *img) {
    if (img == NULL) return;

    // Créer le noyau de taille 3x3
    float **kernel = (float **) malloc(3 * sizeof(float *));
    if (kernel == NULL) return;

    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *) malloc(3 * sizeof(float));
        if (kernel[i] == NULL) {
            // Libérer la mémoire déjà allouée en cas d'erreur
            for (int j = 0; j < i; j++) {
                free(kernel[j]);
            }
            free(kernel);
            return;
        }

        // Initialiser avec 1/9 pour chaque élément
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }

    // Appliquer le filtre à l'image
    bmp8_applyFilter(img, kernel, 3);

    // Libérer la mémoire du noyau
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Applique un flou gaussien à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_gaussian_blur(t_bmp8 *img) {
    if (img == NULL) return;

    // Créer le noyau de taille 3x3
    float **kernel = (float **) malloc(3 * sizeof(float *));
    if (kernel == NULL) return;

    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *) malloc(3 * sizeof(float));
        if (kernel[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(kernel[j]);
            }
            free(kernel);
            return;
        }
    }

    // Matrice de flou gaussien
    kernel[0][0] = 1.0f / 16.0f;
    kernel[0][1] = 2.0f / 16.0f;
    kernel[0][2] = 1.0f / 16.0f;
    kernel[1][0] = 2.0f / 16.0f;
    kernel[1][1] = 4.0f / 16.0f;
    kernel[1][2] = 2.0f / 16.0f;
    kernel[2][0] = 1.0f / 16.0f;
    kernel[2][1] = 2.0f / 16.0f;
    kernel[2][2] = 1.0f / 16.0f;

    // Appliquer le filtre à l'image
    bmp8_applyFilter(img, kernel, 3);

    // Libérer la mémoire du noyau
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Détecte les contours d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_outline(t_bmp8 *img) {
    if (img == NULL) return;

    // Créer le noyau de taille 3x3
    float **kernel = (float **) malloc(3 * sizeof(float *));
    if (kernel == NULL) return;

    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *) malloc(3 * sizeof(float));
        if (kernel[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(kernel[j]);
            }
            free(kernel);
            return;
        }
    }

    // Matrice de détection de contours
    kernel[0][0] = -1.0f;
    kernel[0][1] = -1.0f;
    kernel[0][2] = -1.0f;
    kernel[1][0] = -1.0f;
    kernel[1][1] = 8.0f;
    kernel[1][2] = -1.0f;
    kernel[2][0] = -1.0f;
    kernel[2][1] = -1.0f;
    kernel[2][2] = -1.0f;

    // Appliquer le filtre à l'image
    bmp8_applyFilter(img, kernel, 3);

    // Libérer la mémoire du noyau
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Applique un effet de relief à une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_emboss(t_bmp8 *img) {
    if (img == NULL) return;

    // Créer le noyau de taille 3x3
    float **kernel = (float **) malloc(3 * sizeof(float *));
    if (kernel == NULL) return;

    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *) malloc(3 * sizeof(float));
        if (kernel[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(kernel[j]);
            }
            free(kernel);
            return;
        }
    }

    // Matrice d'effet de relief
    kernel[0][0] = -2.0f;
    kernel[0][1] = -1.0f;
    kernel[0][2] = 0.0f;
    kernel[1][0] = -1.0f;
    kernel[1][1] = 1.0f;
    kernel[1][2] = 1.0f;
    kernel[2][0] = 0.0f;
    kernel[2][1] = 1.0f;
    kernel[2][2] = 2.0f;

    // Appliquer le filtre à l'image
    bmp8_applyFilter(img, kernel, 3);

    // Libérer la mémoire du noyau
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

/**
 * Améliore la netteté d'une image BMP 8 bits
 *
 * @param img L'image à modifier
 */
void bmp8_sharpen(t_bmp8 *img) {
    if (img == NULL) return;

    // Créer le noyau de taille 3x3
    float **kernel = (float **) malloc(3 * sizeof(float *));
    if (kernel == NULL) return;

    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *) malloc(3 * sizeof(float));
        if (kernel[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(kernel[j]);
            }
            free(kernel);
            return;
        }
    }

    // Matrice d'amélioration de la netteté
    kernel[0][0] = 0.0f;
    kernel[0][1] = -1.0f;
    kernel[0][2] = 0.0f;
    kernel[1][0] = -1.0f;
    kernel[1][1] = 5.0f;
    kernel[1][2] = -1.0f;
    kernel[2][0] = 0.0f;
    kernel[2][1] = -1.0f;
    kernel[2][2] = 0.0f;

    // Appliquer le filtre à l'image
    bmp8_applyFilter(img, kernel, 3);

    // Libérer la mémoire du noyau
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}
