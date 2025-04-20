#include <stdio.h>

#include "bmp8.h"

int main(void) {
    t_bmp8 *img = bmp8_loadImage("../images/lena_gray.bmp");

    bmp8_printInfo(img);
    bmp8_brightness(img, 50);
    bmp8_saveImage("../images/lena_gray_brightness.bmp", img);
    bmp8_threshold(img, 128);
    bmp8_saveImage("../images/lena_gray_threshold.bmp", img);
    bmp8_free(img);

    return 0;
}