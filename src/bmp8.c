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
    if (fclose(file) == EOF) { // EOF is a macro that represents the end-of-file
        fprintf(stderr, "Cannot close file\n");
        free(img->data);
        free(img);
        return NULL;
    }

    return img;
}

// Save a BMP8 image to a file
// Source : https://koor.fr/C/cstdio/fwrite.wp
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Cannot save NULL image\n");
        return;
    }

    char path[512];
    strcpy(path, "../images/");
    strcat(path, filename);

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

    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
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

// Apply a filter to a BMP8 image using a kernel
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL || kernel == NULL) {
        fprintf(stderr, "Cannot apply filter to NULL image or with NULL kernel\n");
        return;
    }

    // Verify the kernel size
    if (kernelSize % 2 == 0) {
        fprintf(stderr, "Kernel size must be odd\n");
        return;
    }

    // Calculate the half size of the kernel
    int n = kernelSize / 2;

    // Create a copy of the image data to avoid modifying the original during convolution
    unsigned char *dataCopy = (unsigned char *) malloc(img->dataSize * sizeof(unsigned char));
    if (dataCopy == NULL) {
        fprintf(stderr, "Memory allocation error for image data copy\n");
        return;
    }
    memcpy(dataCopy, img->data, img->dataSize * sizeof(unsigned char));

    // Iterate through each pixel in the image data
    for (unsigned int y = n; y < img->height - n; y++) {
        for (unsigned int x = n; x < img->width - n; x++) {
            // Calculate the index of the pixel in the image data
            unsigned int index = y * img->width + x;

            // Apply the kernel to the pixel
            float sum = 0.0f;
            for (int i = -n; i <= n; i++) {
                for (int j = -n; j <= n; j++) {
                    // Calculate the coordinates of the neighbor pixel
                    unsigned int neighborX = x - j;
                    unsigned int neighborY = y - i;
                    unsigned int neighborIndex = neighborY * img->width + neighborX;

                    // Get the value of the neighbor pixel
                    unsigned char neighborValue = dataCopy[neighborIndex];

                    // Map the kernel coordinates to the kernel array
                    int kernel_i = i + n;
                    int kernel_j = j + n;

                    // Apply the kernel value to the neighbor pixel value
                    sum += neighborValue * kernel[kernel_i][kernel_j];
                }
            }

            // Validate the sum to ensure it stays within the range [0, 255]
            if (sum > 255.0f) {
                sum = 255.0f;
            } else if (sum < 0.0f) {
                sum = 0.0f;
            }

            // Define the new pixel value
            img->data[index] = (unsigned char) sum;
        }
    }

    // Free the copy of the image data
    free(dataCopy);
}
