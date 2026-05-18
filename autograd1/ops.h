#ifndef OPS_H
#define OPS_H

#include "Tensor.h"

Tensor operator+(Tensor& a, Tensor& b);
Tensor operator-(Tensor& a, Tensor& b);
Tensor operator*(Tensor& a, Tensor& b);
Tensor operator/(Tensor& a, Tensor& b);

Tensor matmul(Tensor& a, Tensor& b);
Tensor relu(Tensor& x);
Tensor sigmoid(Tensor& x);
Tensor mseLoss(Tensor& pred, Tensor& target);

#endif
