#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "bmp8.h"
#include "color.h"

/**
 * Calcule l'histogramme d'une image BMP 8 bits
 *
 * @param img L'image dont on souhaite calculer l'histogramme
 * @return unsigned int*: Tableau de 256 éléments contenant les occurrences de chaque niveau de gris
 */
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

/**
 * Calcule la fonction de distribution cumulative (CDF) d'un histogramme
 *
 * @param hist L'histogramme original
 * @return unsigned int*: Histogramme égalisé basé sur la CDF
 */
unsigned int *bmp8_computeCDF(unsigned int *hist);

/**
 * Égalise l'histogramme d'une image BMP 8 bits pour améliorer son contraste
 *
 * @param img L'image à égaliser
 */
void bmp8_equalize(t_bmp8 *img);

/**
 * Égalise l'histogramme d'une image BMP 24 bits pour améliorer son contraste
 * en utilisant l'espace colorimétrique YUV
 *
 * @param img L'image à égaliser
 */
void bmp24_equalize(t_bmp24 *img);

#endif
