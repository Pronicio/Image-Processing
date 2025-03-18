#include <stdio.h>

#include "bmp8.h"

int main(void) {
    t_bmp8 *img = bmp8_loadImage("../images/lena_gray.bmp");

    bmp8_printInfo(img);
    bmp8_free(img);

    return 0;
}