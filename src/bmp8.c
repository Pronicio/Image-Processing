#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load a BMP8 image from a file
// Source : https://koor.fr/C/cstdio/fread.wp
t_bmp8 *bmp8_loadImage(const char *filename) {
    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    FILE *file = fopen(path, "rb"); // Open in binary read mode

    if (file == NULL) {
        fprintf(stderr, "File not found\n");
        return NULL;
    }

    // Allocate memory for the image
    t_bmp8 *img = (t_bmp8 *) malloc(sizeof(t_bmp8));
    if (img == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return NULL;
    }

    // Read the header (54 bytes)
    if (fread(img->header, 1, 54, file) != 54) {
        fprintf(stderr, "Error reading header\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Check if it's a BMP file (header should start with 'BM')
    if (img->header[0] != 'B' || img->header[1] != 'M') {
        fprintf(stderr, "Not a valid BMP file\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Extract image info from header
    // Source : TABLE 1 - BMP image header structure
    img->width = *(unsigned int *) &img->header[18];
    img->height = *(unsigned int *) &img->header[22];
    img->colorDepth = *(unsigned short *) &img->header[28];
    img->dataSize = *(unsigned int *) &img->header[34];

    // Check if it's an 8-bit image
    if (img->colorDepth != 8) {
        fprintf(stderr, "Not an 8-bit BMP image (color depth is %d bits)\n", img->colorDepth);
        free(img);
        fclose(file);
        return NULL;
    }

    if (fread(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Error reading color table\n");
        free(img->colorTable);
        free(img->header);
        free(img);
        fclose(file);
        return NULL;
    }

    // Read the image data
    img->data = (unsigned char *) malloc(img->dataSize * sizeof(unsigned char));
    if (img->data == NULL) {
        fprintf(stderr, "Memory allocation error for image data\n");
        free(img);
        fclose(file);
        return NULL;
    }

    if (fread(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error reading image data\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    // Close the file
    if (fclose(file) == EOF) {
        // EOF is a macro that represents the end-of-file
        fprintf(stderr, "Cannot close file\n");
        free(img->data);
        free(img);
        return NULL;
    }

    return img;
}

// Save a BMP8 image to a file
// Source : https://koor.fr/C/cstdio/fwrite.wp
void bmp8_saveImage(t_bmp8 *img, const char *filename) {
    if (img == NULL) {
        fprintf(stderr, "Cannot save NULL image\n");
        return;
    }

    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

    printf("%s \n\n", path);

    FILE *file = fopen(path, "wb"); // Open in binary write mode

    if (file == NULL) {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return;
    }

    fwrite(img->header, 1, 54, file);
    fwrite(img->colorTable, 1, 1024, file);
    fwrite(img->data, 1, img->dataSize, file);
    fclose(file);
}

// Free the memory allocated for a BMP8 image
// Source : https://koor.fr/C/cstdlib/free.wp
void bmp8_free(t_bmp8 *img) {
    if (img == NULL) { return; }
    free(img->data);
    free(img);
}

// Print information about a BMP8 image
void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Cannot display info for NULL image\n");
        return;
    }

    printf("📐 Width: %u\n", img->width);
    printf("📐 Height: %u\n", img->height);
    printf("🎨 Color Depth: %u\n", img->colorDepth);
    printf("🖼️ Data Size: %u\n", img->dataSize);
}

// Apply a negative effect to a BMP8 image
void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Cannot apply negative effect to NULL image\n");
        return;
    }

    // Iterate through each pixel in the image data
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Invert pixel value (255 - value)
        img->data[i] = 255 - img->data[i];
    }
}

// Modify the luminance of a BMP8 image
void bmp8_brightness(const t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Cannot adjust brightness for NULL image\n");
        return;
    }

    // Iterate through each pixel in the image data
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Calculate the new pixel value
        int newPixelValue = img->data[i] + value;

        // Ensure the value is within the valid range [0, 255]
        if (newPixelValue > 255) {
            newPixelValue = 255;
        } else if (newPixelValue < 0) {
            newPixelValue = 0;
        }

        // Assign the new pixel value
        img->data[i] = (unsigned char) newPixelValue;
    }
}

// Transform a BMP8 image to its negative
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Cannot apply threshold to NULL image\n");
        return;
    }

    // Iterate through each pixel in the image data
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        // Apply the threshold
        if (img->data[i] >= threshold) {
            img->data[i] = 255; // White if value >= threshold
        } else {
            img->data[i] = 0; // Black if value < threshold
        }
    }
}

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

// Box blur (flou uniforme)
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

// Gaussian blur (flou gaussien)
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

// Outline (détection de contours)
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

// Emboss (relief)
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

// Sharpen (netteté)
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
