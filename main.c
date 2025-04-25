#include <stdio.h>

#include "./src/bmp8.h"
#include "./src/bmp24.h"

int main(void) {
    // Loading an 8-bit BMP image
    t_bmp8 *img = bmp8_loadImage("../images/lena_gray.bmp");

    bmp8_printInfo(img);

    bmp8_negative(img);
    bmp8_saveImage("../images/lena_gray_negative.bmp", img);

    bmp8_brightness(img, 50);
    bmp8_saveImage("../images/lena_gray_brightness.bmp", img);

    bmp8_threshold(img, 128);
    bmp8_saveImage("../images/lena_gray_threshold.bmp", img);

    bmp8_free(img);
    
    // Loading a 24-bit BMP image
    printf("Loading the image...\n");
    t_bmp24 *img24 = bmp24_loadImage("../images/lena_color.bmp");
    if (img24 == NULL) {
        fprintf(stderr, "Error while loading the image\n");
        return 1;
    }

    printf("Image loaded successfully:\n");
    printf("Width: %d, Height: %d, Depth: %d bits\n", img24->width, img24->height, img24->colorDepth);

    // Testing the brightness function
    printf("Testing the brightness function...\n");
    t_bmp24 *img_bright = bmp24_loadImage("../images/lena_color.bmp");
    bmp24_brightness(img_bright, 50);
    bmp24_saveImage(img_bright, "../images/lena_color_brightness.bmp");
    bmp24_free(img_bright);
    
    // Testing the grayscale conversion function
    printf("Converting to grayscale...\n");
    t_bmp24 *img_gray = bmp24_loadImage("../images/lena_color.bmp");
    bmp24_grayscale(img_gray);
    bmp24_saveImage(img_gray, "../images/lena_color_grayscale.bmp");
    bmp24_free(img_gray);

    // Testing the negative function
    printf("Creating negative...\n");
    t_bmp24 *img_neg = bmp24_loadImage("../images/lena_color.bmp");
    bmp24_negative(img_neg);
    bmp24_saveImage(img_neg, "../images/lena_color_negative.bmp");
    bmp24_free(img_neg);

    // Combined test: grayscale + brightness
    printf("Combined test: grayscale + brightness...\n");
    t_bmp24 *img_combo = bmp24_loadImage("../images/lena_color.bmp");
    bmp24_grayscale(img_combo);
    bmp24_brightness(img_combo, -30); // Changing brightness after grayscale conversion
    bmp24_saveImage(img_combo, "../images/lena_color_gray_dark.bmp");
    bmp24_free(img_combo);

    // Freeing the original image
    bmp24_free(img24);

    return 0;
}
