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

void bmp8_equalize(t_bmp8 *img) {
    unsigned int *hist = bmp8_computeHistogram(img);
    unsigned int *hist_eq = bmp8_computeCDF(hist);

    for (int i = 0; i < img->dataSize - 1; i++) {
        img->data[i] = hist_eq[img->data[i]];
    }
}

void bmp24_equalize(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        return;
    }

    // Number of pixels in the image
    unsigned int pixelCount = img->width * img->height;

    // 1. Convert RGB to YUV
    float *Y = (float *) malloc(pixelCount * sizeof(float));
    float *U = (float *) malloc(pixelCount * sizeof(float));
    float *V = (float *) malloc(pixelCount * sizeof(float));

    if (Y == NULL || U == NULL || V == NULL) {
        if (Y) free(Y);
        if (U) free(U);
        if (V) free(V);
        return;
    }

    // RGB -> YUV
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned int idx = y * img->width + x;
            unsigned char R = img->data[y][x].red;
            unsigned char G = img->data[y][x].green;
            unsigned char B = img->data[y][x].blue;

            Y[idx] = 0.299 * R + 0.587 * G + 0.114 * B;
            U[idx] = -0.14713 * R - 0.28886 * G + 0.436 * B;
            V[idx] = 0.615 * R - 0.51499 * G - 0.10001 * B;
        }
    }

    // 2. Calculate the histogram of the Y component
    unsigned int hist[256] = {0};
    for (unsigned int i = 0; i < pixelCount; i++) {
        unsigned char y = (unsigned char) (Y[i] < 0 ? 0 : (Y[i] > 255 ? 255 : Y[i]));
        hist[y]++;
    }

    // 3. Calculate the CDF
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

    // 3. Normalize the CDF
    unsigned int hist_eq[256];
    for (int i = 0; i < 256; i++) {
        if (cdf[i] < cdf_min) {
            hist_eq[i] = 0;
        } else {
            hist_eq[i] = (unsigned int) round(((double) (cdf[i] - cdf_min) / (double) (pixelCount - cdf_min)) * 255.0);
        }
    }

    // 4. Apply the histogram equalization to the Y component
    for (unsigned int i = 0; i < pixelCount; i++) {
        unsigned char y = (unsigned char) (Y[i] < 0 ? 0 : (Y[i] > 255 ? 255 : Y[i]));
        Y[i] = hist_eq[y];
    }

    // 5. Reconvert YUV to RGB
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            unsigned int idx = y * img->width + x;
            float y_val = Y[idx];
            float u = U[idx];
            float v = V[idx];

            int r = (int) (y_val + 1.13983 * v);
            int g = (int) (y_val - 0.39465 * u - 0.58060 * v);
            int b = (int) (y_val + 2.03211 * u);

            // Limit the values to the range [0, 255]
            r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
            g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
            b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

            // Update the pixel values
            img->data[y][x].red = (unsigned char) r;
            img->data[y][x].green = (unsigned char) g;
            img->data[y][x].blue = (unsigned char) b;
        }
    }

    free(Y);
    free(U);
    free(V);
}