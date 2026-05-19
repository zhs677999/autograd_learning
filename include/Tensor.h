#ifndef TENSOR_H
#define TENSOR_H

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
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

    void buildTopo(vector<Tensor*>& topo, vector<Tensor*>& visited) {
        for (int i = 0; i < static_cast<int>(visited.size()); i++) {
            if (visited[i] == this) {
                return;
            }
        }

        visited.push_back(this);
        for (int i = 0; i < static_cast<int>(parents.parentCount()); i++) {
            Tensor* parent = parents.getParent(i);
            if (parent) {
                parent->buildTopo(topo, visited);
            }
        }
        topo.push_back(this);
    }

public:
    Tensor() {
        data = nullptr;
        grad = nullptr;
        rows = 0;
        cols = 0;
        backwardFunc = nullptr;
        requiresGrad = true;
    }

    Tensor(double value, bool reqGrad = true) {
        data = new double[1];
        grad = new double[1];
        data[0] = value;
        grad[0] = 0.0;
        rows = 1;
        cols = 1;
        backwardFunc = nullptr;
        requiresGrad = reqGrad;
    }

    Tensor(int r, int c, double initValue = 0.0, bool reqGrad = true) {
        if (r <= 0 || c <= 0) {
            throw invalid_argument("Tensor shape must be positive");
        }

        rows = r;
        cols = c;
        data = new double[rows * cols];
        grad = new double[rows * cols];
        for (int i = 0; i < rows * cols; i++) {
            data[i] = initValue;
            grad[i] = 0.0;
        }
        backwardFunc = nullptr;
        requiresGrad = reqGrad;
    }

    Tensor(const Tensor& other) {
        rows = other.rows;
        cols = other.cols;
        requiresGrad = other.requiresGrad;
        backwardFunc = other.backwardFunc;

        int n = rows * cols;
        data = n > 0 ? new double[n] : nullptr;
        grad = n > 0 ? new double[n] : nullptr;
        for (int i = 0; i < n; i++) {
            data[i] = other.data[i];
            grad[i] = other.grad[i];
        }

        for (int i = 0; i < static_cast<int>(other.parents.parentCount()); i++) {
            parents.add(other.parents.getParent(i));
        }
    }

    Tensor& operator=(const Tensor& other) {
        if (this == &other) {
            return *this;
        }

        delete[] data;
        delete[] grad;

        rows = other.rows;
        cols = other.cols;
        requiresGrad = other.requiresGrad;
        backwardFunc = other.backwardFunc;

        int n = rows * cols;
        data = n > 0 ? new double[n] : nullptr;
        grad = n > 0 ? new double[n] : nullptr;
        for (int i = 0; i < n; i++) {
            data[i] = other.data[i];
            grad[i] = other.grad[i];
        }

        parents.clear();
        for (int i = 0; i < static_cast<int>(other.parents.parentCount()); i++) {
            parents.add(other.parents.getParent(i));
        }
        return *this;
    }

    ~Tensor() {
        delete[] data;
        delete[] grad;
    }

    int rowCount() const {
        return rows;
    }

    int colCount() const {
        return cols;
    }

    int size() const {
        return rows * cols;
    }

    double& at(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            throw out_of_range("Index out of bounds");
        }
        return data[r * cols + c];
    }

    double at(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            throw out_of_range("Index out of bounds");
        }
        return data[r * cols + c];
    }

    double value() const {
        if (rows != 1 || cols != 1) {
            throw logic_error("Tensor is not a single element");
        }
        return data[0];
    }

    double gradValue() const {
        if (rows != 1 || cols != 1) {
            throw logic_error("Tensor is not a single element");
        }
        return grad[0];
    }

    void setGrad(int index, double value) {
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        grad[index] = value;
    }

    double getGrad(int index) const {
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        return grad[index];
    }

    void addGrad(int index, double value) {
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        grad[index] += value;
    }

    void zeroGrad() {
        for (int i = 0; i < size(); i++) {
            grad[i] = 0.0;
        }
    }

    void addParent(Tensor* p) {
        parents.add(p);
    }

    Tensor* getParent(int i) const {
        return parents.getParent(i);
    }

    int parentCount() const {
        return static_cast<int>(parents.parentCount());
    }

    void setBackwardFunc(BackwardFunc f) {
        backwardFunc = f;
    }

    void backward() {
        if (rows != 1 || cols != 1) {
            throw logic_error("backward only supports scalar Tensor in basic tier");
        }
        setGrad(0, 1.0);
        _backward();
    }

    void _backward() {
        vector<Tensor*> topo;
        vector<Tensor*> visited;
        buildTopo(topo, visited);

        for (int i = static_cast<int>(topo.size()) - 1; i >= 0; i--) {
            Tensor* node = topo[i];
            if (node->backwardFunc) {
                node->backwardFunc(node);
            }
        }
    }

    bool needGrad() const {
        return requiresGrad;
    }

    void print() const {
        cout << "Tensor(" << rows << "x" << cols << "): [";
        for (int i = 0; i < rows; i++) {
            cout << "[";
            for (int j = 0; j < cols; j++) {
                cout << data[i * cols + j];
                if (j < cols - 1) {
                    cout << ", ";
                }
            }
            cout << "]";
            if (i < rows - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }

    void printGrad() const {
        cout << "Gradient(" << rows << "x" << cols << "): [";
        for (int i = 0; i < rows; i++) {
            cout << "[";
            for (int j = 0; j < cols; j++) {
                cout << grad[i * cols + j];
                if (j < cols - 1) {
                    cout << ", ";
                }
            }
            cout << "]";
            if (i < rows - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }

    friend Tensor operator+(Tensor& a, Tensor& b);
    friend Tensor operator-(Tensor& a, Tensor& b);
    friend Tensor operator*(Tensor& a, Tensor& b);
    friend Tensor operator/(Tensor& a, Tensor& b);

    friend Tensor matmul(Tensor& a, Tensor& b);
    friend Tensor relu(Tensor& x);
    friend Tensor sigmoid(Tensor& x);
};

#endif
