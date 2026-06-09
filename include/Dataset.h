#ifndef DATASET_H
#define DATASET_H

#include "Tensor.h"

void generateConcentricCircles(Tensor& x, Tensor& y,
    int numSamples,
    double radiusInner = 1.0,
    double radiusOuter = 2.0,
    double noise = 0.05);

#endif
