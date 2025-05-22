#include "histogram.h"

#include <stdlib.h>
#include <math.h>

unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (img == NULL) {
        return NULL;
    }

    // Memory allocation for the histogram
    // The histogram will have 256 bins (0-255)
    unsigned int *histogram = (unsigned int *) malloc(256 * sizeof(unsigned int));

    // Initialize the histogram to zero
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    // Map the pixel values to the histogram
    for (unsigned int i = 0; i < img->dataSize; i++) {
        histogram[img->data[i]]++;
    }

    return histogram;
}

unsigned int *bmp8_computeCDF(unsigned int *hist) {
    if (hist == NULL) {
        return NULL;
    }

    // Allocate memory for the equalized histogram
    unsigned int *hist_eq = (unsigned int *) malloc(256 * sizeof(unsigned int));

    // Calculate the CDF
    unsigned int cdf[256];
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    // Find cdf_min (the first non-zero value in the CDF)
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // N : total number of pixels
    unsigned int N = cdf[255];

    // Normalize the CDF to obtain
    for (int i = 0; i < 256; i++) {
        if (cdf[i] < cdf_min) {
            hist_eq[i] = 0;
        } else {
            // Normalization formula
            hist_eq[i] = (unsigned int) round(((double) (cdf[i] - cdf_min) / (double) (N - cdf_min)) * 255.0);
        }
    }

    return hist_eq;
}

void bmp8_equalize(t_bmp8 * img) {
    unsigned int *hist = bmp8_computeHistogram(img);
    unsigned int *hist_eq = bmp8_computeCDF(hist);

    for (int i = 0; i < img->dataSize-1; i++) {
        img->data[i] = hist_eq[img->data[i]];
    }
}