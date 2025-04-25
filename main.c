#include <stdio.h>

#include "./src/bmp8.h"

int main(void) {
    // Load a 8-bit BMP image
    t_bmp8 *img = bmp8_loadImage("../images/lena_gray.bmp");

    bmp8_printInfo(img);

    bmp8_negative(img);
    bmp8_saveImage("../images/lena_gray_negative.bmp", img);

    bmp8_brightness(img, 50);
    bmp8_saveImage("../images/lena_gray_brightness.bmp", img);

    bmp8_threshold(img, 128);
    bmp8_saveImage("../images/lena_gray_threshold.bmp", img);

    bmp8_free(img);

    return 0;
}