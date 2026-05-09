#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Tensor.h"
#include "LinkedList.h"

class SGD {
private:
    LinkedList<Tensor*> params;
    double lr;

public:
    SGD(double learningRate);

    void addParam(Tensor* p);
    void step();
    void zeroGrad();
};

#endif
