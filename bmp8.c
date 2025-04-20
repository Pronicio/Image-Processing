#include <stdio.h>
#include <stdlib.h>

#include "bmp8.h"

// Load a BMP8 image from a file
// Source : https://koor.fr/C/cstdio/fread.wp
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Open in binary read mode

    if (file == NULL) {
        fprintf(stderr, "File not found\n");
        return NULL;
    }

    // Allocate memory for the image
    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (img == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return NULL;
    }

    // Read the header (54 bytes)
    if (fread(img->header, sizeof(unsigned char), 54, file) != 54) {
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
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (img == NULL) {
        fprintf(stderr, "Cannot save NULL image\n");
        return;
    }

    FILE *file = fopen(filename, "wb"); // Open in binary write mode

    if (file == NULL) {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return;
    }

    // Write the header (54 bytes)
    if (fwrite(img->header, sizeof(unsigned char), 54, file) != 54) {
        fprintf(stderr, "Error writing header\n");
        fclose(file);
        return;
    }

    // Write the color table (256 colors × 4 bytes = 1024 bytes)
    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        fprintf(stderr, "Error writing color table\n");
        fclose(file);
        return;
    }

    // Write the image data
    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error writing image data\n");
        fclose(file);
        return;
    }

    // Close the file
    if (fclose(file) == EOF) {
        fprintf(stderr, "Cannot close file: %s\n", filename);
    }
}

// Free the memory allocated for a BMP8 image
// Source : https://koor.fr/C/cstdlib/free.wp
void bmp8_free(t_bmp8 *img) {
    if (img == NULL) {
        return; // Nothing to free
    }

    // Free the image data if it exists
    if (img->data != NULL) {
        free(img->data);
        img->data = NULL;
    }

    // Free the image structure itself
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

// Modify the luminance of a BMP8 image
void bmp8_brightness(const t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        fprintf(stderr, "Cannot adjust brightness for NULL image\n");
        return;
    }

    // Iterate through each pixel in the image data
    for (unsigned int i = 0; i < img->dataSize; i++) {
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
    for (unsigned int i = 0; i < img->dataSize; i++) {
        // Apply the threshold
        if (img->data[i] >= threshold) {
            img->data[i] = 255; // White if value >= threshold
        } else {
            img->data[i] = 0; // Black if value < threshold
        }
    }
}
