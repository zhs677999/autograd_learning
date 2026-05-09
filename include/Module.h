#ifndef MODULE_H
#define MODULE_H

#include "Tensor.h"
#include "LinkedList.h"

class Linear {
private:
    Tensor W;
    Tensor b;

public:
    Linear(int inFeatures, int outFeatures);

    Tensor forward(Tensor& x);

    Tensor& weight();
    Tensor& bias();

    void zeroGrad();
};

#endif
