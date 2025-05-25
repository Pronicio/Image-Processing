#include "histogram.h"

#include <stdlib.h>
#include <math.h>

/**
 * Calcule l'histogramme d'une image BMP 8 bits
 *
 * @param img L'image dont on souhaite calculer l'histogramme
 * @return unsigned int*: Tableau de 256 éléments contenant les occurrences de chaque niveau de gris
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (img == NULL) {
        return NULL;
    }

    // Allocation de mémoire pour l'histogramme
    // L'histogramme aura 256 compartiments (0-255)
    unsigned int *histogram = (unsigned int *) malloc(256 * sizeof(unsigned int));

    // Initialisation de l'histogramme à zéro
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    // Associer les valeurs des pixels à l'histogramme
    for (unsigned int i = 0; i < img->dataSize; i++) {
        histogram[img->data[i]]++;
    }

    return histogram;
}

/**
 * Calcule la fonction de distribution cumulative (CDF) d'un histogramme
 *
 * @param hist L'histogramme original
 * @return unsigned int*: Histogramme égalisé basé sur la CDF
 */
unsigned int *bmp8_computeCDF(unsigned int *hist) {
    if (hist == NULL) {
        return NULL;
    }

    // Allouer de la mémoire pour l'histogramme égalisé
    unsigned int *hist_eq = (unsigned int *) malloc(256 * sizeof(unsigned int));

    // Calculer la CDF (Fonction de Distribution Cumulative)
    unsigned int cdf[256];
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Trouver cdf_min (la première valeur non nulle dans la CDF)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // N : nombre total de pixels
    unsigned int N = cdf[255];

    // Normaliser la CDF pour obtenir l'histogramme égalisé
    for (int i = 0; i < 256; i++) {
        if (cdf[i] < cdf_min) {
            hist_eq[i] = 0;
        } else {
            // Formule de normalisation
            hist_eq[i] = (unsigned int) round(((double) (cdf[i] - cdf_min) / (double) (N - cdf_min)) * 255.0);
        }
    }

    return hist_eq;
}

/**
 * Égalise l'histogramme d'une image BMP 8 bits pour améliorer son contraste
 *
 * @param img L'image à égaliser
 */
void bmp8_equalize(t_bmp8 *img) {
    // Calcul de l'histogramme de l'image
    unsigned int *hist = bmp8_computeHistogram(img);
    
    // Calcul de l'histogramme égalisé
    unsigned int *hist_eq = bmp8_computeCDF(hist);

    // Application de l'égalisation à chaque pixel de l'image
    for (int i = 0; i < img->dataSize - 1; i++) {
        img->data[i] = hist_eq[img->data[i]];
    }
    
    // Libération de la mémoire
    free(hist);
    free(hist_eq);
}

/**
 * Égalise l'histogramme d'une image BMP 24 bits pour améliorer son contraste
 * en utilisant l'espace colorimétrique YUV
 *
 * @param img L'image à égaliser
 */
void bmp24_equalize(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        return;
    }

    // Nombre de pixels dans l'image
    unsigned int pixelCount = img->width * img->height;

    // 1. Conversion RGB vers YUV (Y: luminance, U,V: chrominance)
    float *Y = (float *) malloc(pixelCount * sizeof(float));
    float *U = (float *) malloc(pixelCount * sizeof(float));
    float *V = (float *) malloc(pixelCount * sizeof(float));

    if (Y == NULL || U == NULL || V == NULL) {
        if (Y) free(Y);
        if (U) free(U);
        if (V) free(V);
        return;
    }

    // RGB -> YUV (formule standard de conversion)
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned int idx = y * img->width + x;
            unsigned char R = img->data[y][x].red;
            unsigned char G = img->data[y][x].green;
            unsigned char B = img->data[y][x].blue;

            Y[idx] = 0.299 * R + 0.587 * G + 0.114 * B;          // Luminance
            U[idx] = -0.14713 * R - 0.28886 * G + 0.436 * B;     // Chrominance bleue
            V[idx] = 0.615 * R - 0.51499 * G - 0.10001 * B;      // Chrominance rouge
        }
    }

    // 2. Calcul de l'histogramme de la composante Y (luminance)
    unsigned int hist[256] = {0};
    for (unsigned int i = 0; i < pixelCount; i++) {
        unsigned char y = (unsigned char) (Y[i] < 0 ? 0 : (Y[i] > 255 ? 255 : Y[i]));
        hist[y]++;
    }

    // 3. Calcul de la CDF (Fonction de Distribution Cumulative)
    unsigned int cdf[256];
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Trouver cdf_min (la première valeur non nulle dans la CDF)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // 3. Normalisation de la CDF pour obtenir l'histogramme égalisé
    unsigned int hist_eq[256];
    for (int i = 0; i < 256; i++) {
        if (cdf[i] < cdf_min) {
            hist_eq[i] = 0;
        } else {
            hist_eq[i] = (unsigned int) round(((double) (cdf[i] - cdf_min) / (double) (pixelCount - cdf_min)) * 255.0);
        }
    }

    // 4. Application de l'égalisation à la composante Y uniquement
    for (unsigned int i = 0; i < pixelCount; i++) {
        unsigned char y = (unsigned char) (Y[i] < 0 ? 0 : (Y[i] > 255 ? 255 : Y[i]));
        Y[i] = hist_eq[y];
    }

    // 5. Reconversion YUV vers RGB
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned int idx = y * img->width + x;
            float y_val = Y[idx];
            float u = U[idx];
            float v = V[idx];

            // Formules de conversion YUV vers RGB
            int r = (int) (y_val + 1.13983 * v);
            int g = (int) (y_val - 0.39465 * u - 0.58060 * v);
            int b = (int) (y_val + 2.03211 * u);

            // Limiter les valeurs à l'intervalle [0, 255]
            r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
            g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
            b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

            // Mise à jour des valeurs des pixels
            img->data[y][x].red = (unsigned char) r;
            img->data[y][x].green = (unsigned char) g;
            img->data[y][x].blue = (unsigned char) b;
        }
    }

    // Libération de la mémoire
    free(Y);
    free(U);
    free(V);
}
