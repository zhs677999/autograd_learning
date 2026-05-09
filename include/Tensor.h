#ifndef TENSOR_H
#define TENSOR_H

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>
#include "LinkedList.h"

using namespace std;

class Tensor;

typedef void (*BackwardFunc)(Tensor* self);

class Tensor {
private:
    double* data;
    double* grad;
    int rows;
    int cols;

    LinkedList<Tensor*> parents;
    BackwardFunc backwardFunc;

    bool requiresGrad;

public:
    Tensor();
    Tensor(double value, bool reqGrad = true);
    Tensor(int r, int c, double initValue = 0.0, bool reqGrad = true);

    Tensor(const Tensor& other);
    Tensor& operator=(const Tensor& other);
    ~Tensor();

    int rowCount() const;
    int colCount() const;
    int size() const;

    double& at(int r, int c);
    double at(int r, int c) const;

    double value() const;
    double gradValue() const;

    void setGrad(int index, double value);
    double getGrad(int index) const;
    void addGrad(int index, double value);

    void zeroGrad();

    void addParent(Tensor* p);
    Tensor* getParent(int i);
    int parentCount() const;

    void setBackwardFunc(BackwardFunc f);
    void backward();
    void _backward();

    bool needGrad() const;

    void print() const;
    void printGrad() const;

    friend Tensor operator+(Tensor& a, Tensor& b);
    friend Tensor operator-(Tensor& a, Tensor& b);
    friend Tensor operator*(Tensor& a, Tensor& b);
    friend Tensor operator/(Tensor& a, Tensor& b);

    friend Tensor matmul(Tensor& a, Tensor& b);
    friend Tensor relu(Tensor& x);
    friend Tensor sigmoid(Tensor& x);
};

#endif
