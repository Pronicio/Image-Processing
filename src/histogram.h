#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "bmp8.h"
#include "../test/color.h"

unsigned int *bmp8_computeHistogram(t_bmp8 *img);

unsigned int *bmp8_computeCDF(unsigned int *hist);

void bmp8_equalize(t_bmp8 *img);

void bmp24_equalize(t_bmp24 *img);

#endif
