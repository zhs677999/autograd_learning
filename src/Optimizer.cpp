#include "Optimizer.h"

SGD::SGD(double learningRate) : lr(learningRate) {}

void SGD::addParam(Tensor* p) {
    if (p) {
        params.add(p);
    }
}

void SGD::step() {
    for (int n = 0; n < static_cast<int>(params.parentCount()); n++) {
        Tensor* p = params.getParent(n);
        if (!p || !p->needGrad()) {
            continue;
        }

        for (int i = 0; i < p->rowCount(); i++) {
            for (int j = 0; j < p->colCount(); j++) {
                int index = i * p->colCount() + j;
                p->at(i, j) -= lr * p->getGrad(index);
            }
        }
    }
}

void SGD::zeroGrad() {
    for (int n = 0; n < static_cast<int>(params.parentCount()); n++) {
        Tensor* p = params.getParent(n);
        if (p) {
            p->zeroGrad();
        }
    }
}
