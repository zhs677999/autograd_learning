#include "Ops.h"

#include <cmath>
#include <stdexcept>
#include <string>

static int maxInt(int a, int b) {
    return a > b ? a : b;
}

static bool canBroadcast(Tensor& a, Tensor& b) {
    bool rowsOk = a.rowCount() == b.rowCount() || a.rowCount() == 1 || b.rowCount() == 1;
    bool colsOk = a.colCount() == b.colCount() || a.colCount() == 1 || b.colCount() == 1;
    return rowsOk && colsOk;
}

static void requireBroadcastable(Tensor& a, Tensor& b, const char* opName) {
    if (!canBroadcast(a, b)) {
        throw std::invalid_argument(std::string(opName) + " requires broadcastable Tensor shapes");
    }
}

static int broadcastRows(Tensor& a, Tensor& b) {
    return maxInt(a.rowCount(), b.rowCount());
}

static int broadcastCols(Tensor& a, Tensor& b) {
    return maxInt(a.colCount(), b.colCount());
}

static int sourceRow(Tensor* t, int row) {
    return t->rowCount() == 1 ? 0 : row;
}

static int sourceCol(Tensor* t, int col) {
    return t->colCount() == 1 ? 0 : col;
}

static int sourceIndex(Tensor* t, int row, int col) {
    return sourceRow(t, row) * t->colCount() + sourceCol(t, col);
}

static double broadcastValue(Tensor* t, int row, int col) {
    return t->at(sourceRow(t, row), sourceCol(t, col));
}

static void addBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double upstream = self->getGrad(i * self->colCount() + j);
            if (a && a->needGrad()) {
                a->addGrad(sourceIndex(a, i, j), upstream);
            }
            if (b && b->needGrad()) {
                b->addGrad(sourceIndex(b, i, j), upstream);
            }
        }
    }
}

Tensor operator+(Tensor& a, Tensor& b) {
    requireBroadcastable(a, b, "operator+");

    int rows = broadcastRows(a, b);
    int cols = broadcastCols(a, b);
    bool req = a.needGrad() || b.needGrad();
    Tensor out(rows, cols, 0.0, req);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out.at(i, j) = broadcastValue(&a, i, j) + broadcastValue(&b, i, j);
        }
    }

    out.addParent(&a);
    out.addParent(&b);
    if (req) {
        out.setBackwardFunc(addBackward);
    }
    return out;
}

static void subBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double upstream = self->getGrad(i * self->colCount() + j);
            if (a && a->needGrad()) {
                a->addGrad(sourceIndex(a, i, j), upstream);
            }
            if (b && b->needGrad()) {
                b->addGrad(sourceIndex(b, i, j), -upstream);
            }
        }
    }
}

Tensor operator-(Tensor& a, Tensor& b) {
    requireBroadcastable(a, b, "operator-");

    int rows = broadcastRows(a, b);
    int cols = broadcastCols(a, b);
    bool req = a.needGrad() || b.needGrad();
    Tensor out(rows, cols, 0.0, req);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out.at(i, j) = broadcastValue(&a, i, j) - broadcastValue(&b, i, j);
        }
    }

    out.addParent(&a);
    out.addParent(&b);
    if (req) {
        out.setBackwardFunc(subBackward);
    }
    return out;
}

static void mulBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double upstream = self->getGrad(i * self->colCount() + j);
            double aValue = broadcastValue(a, i, j);
            double bValue = broadcastValue(b, i, j);
            if (a && a->needGrad()) {
                a->addGrad(sourceIndex(a, i, j), upstream * bValue);
            }
            if (b && b->needGrad()) {
                b->addGrad(sourceIndex(b, i, j), upstream * aValue);
            }
        }
    }
}

Tensor operator*(Tensor& a, Tensor& b) {
    requireBroadcastable(a, b, "operator*");

    int rows = broadcastRows(a, b);
    int cols = broadcastCols(a, b);
    bool req = a.needGrad() || b.needGrad();
    Tensor out(rows, cols, 0.0, req);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out.at(i, j) = broadcastValue(&a, i, j) * broadcastValue(&b, i, j);
        }
    }

    out.addParent(&a);
    out.addParent(&b);
    if (req) {
        out.setBackwardFunc(mulBackward);
    }
    return out;
}

static void divBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double upstream = self->getGrad(i * self->colCount() + j);
            double aValue = broadcastValue(a, i, j);
            double bValue = broadcastValue(b, i, j);
            if (a && a->needGrad()) {
                a->addGrad(sourceIndex(a, i, j), upstream / bValue);
            }
            if (b && b->needGrad()) {
                b->addGrad(sourceIndex(b, i, j), upstream * (-aValue) / (bValue * bValue));
            }
        }
    }
}

Tensor operator/(Tensor& a, Tensor& b) {
    requireBroadcastable(a, b, "operator/");

    int rows = broadcastRows(a, b);
    int cols = broadcastCols(a, b);
    bool req = a.needGrad() || b.needGrad();
    Tensor out(rows, cols, 0.0, req);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double denominator = broadcastValue(&b, i, j);
            if (denominator == 0.0) {
                throw std::runtime_error("Division by zero in Tensor operation.");
            }
            out.at(i, j) = broadcastValue(&a, i, j) / denominator;
        }
    }

    out.addParent(&a);
    out.addParent(&b);
    if (req) {
        out.setBackwardFunc(divBackward);
    }
    return out;
}

static void mseBackward(Tensor* self) {
    Tensor* pred = self->getParent(0);
    Tensor* target = self->getParent(1);

    int rows = broadcastRows(*pred, *target);
    int cols = broadcastCols(*pred, *target);
    int count = rows * cols;
    double upstream = self->getGrad(0);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double diff = broadcastValue(pred, i, j) - broadcastValue(target, i, j);
            double grad = upstream * 2.0 * diff / count;
            if (pred && pred->needGrad()) {
                pred->addGrad(sourceIndex(pred, i, j), grad);
            }
            if (target && target->needGrad()) {
                target->addGrad(sourceIndex(target, i, j), -grad);
            }
        }
    }
}

Tensor mseLoss(Tensor& pred, Tensor& target) {
    requireBroadcastable(pred, target, "mseLoss");

    int rows = broadcastRows(pred, target);
    int cols = broadcastCols(pred, target);
    int count = rows * cols;
    double sum = 0.0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double diff = broadcastValue(&pred, i, j) - broadcastValue(&target, i, j);
            sum += diff * diff;
        }
    }

    bool req = pred.needGrad() || target.needGrad();
    Tensor out(sum / count, req);
    out.addParent(&pred);
    out.addParent(&target);
    if (req) {
        out.setBackwardFunc(mseBackward);
    }
    return out;
}

static void matmulBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    if (a && a->needGrad()) {
        for (int i = 0; i < a->rowCount(); i++) {
            for (int k = 0; k < a->colCount(); k++) {
                double grad = 0.0;
                for (int j = 0; j < b->colCount(); j++) {
                    grad += self->getGrad(i * self->colCount() + j) * b->at(k, j);
                }
                a->addGrad(i * a->colCount() + k, grad);
            }
        }
    }

    if (b && b->needGrad()) {
        for (int k = 0; k < b->rowCount(); k++) {
            for (int j = 0; j < b->colCount(); j++) {
                double grad = 0.0;
                for (int i = 0; i < a->rowCount(); i++) {
                    grad += a->at(i, k) * self->getGrad(i * self->colCount() + j);
                }
                b->addGrad(k * b->colCount() + j, grad);
            }
        }
    }
}

Tensor matmul(Tensor& a, Tensor& b) {
    if (a.colCount() != b.rowCount()) {
        throw std::invalid_argument("matmul requires a.colCount() == b.rowCount()");
    }

    bool req = a.needGrad() || b.needGrad();
    Tensor out(a.rowCount(), b.colCount(), 0.0, req);

    for (int i = 0; i < a.rowCount(); i++) {
        for (int j = 0; j < b.colCount(); j++) {
            double sum = 0.0;
            for (int k = 0; k < a.colCount(); k++) {
                sum += a.at(i, k) * b.at(k, j);
            }
            out.at(i, j) = sum;
        }
    }

    out.addParent(&a);
    out.addParent(&b);
    if (req) {
        out.setBackwardFunc(matmulBackward);
    }
    return out;
}

static void reluBackward(Tensor* self) {
    Tensor* x = self->getParent(0);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double upstream = self->getGrad(i * self->colCount() + j);
            double localGrad = x->at(i, j) > 0.0 ? 1.0 : 0.0;
            if (x && x->needGrad()) {
                x->addGrad(i * x->colCount() + j, upstream * localGrad);
            }
        }
    }
}

Tensor relu(Tensor& x) {
    bool req = x.needGrad();
    Tensor out(x.rowCount(), x.colCount(), 0.0, req);

    for (int i = 0; i < x.rowCount(); i++) {
        for (int j = 0; j < x.colCount(); j++) {
            double value = x.at(i, j);
            out.at(i, j) = value > 0.0 ? value : 0.0;
        }
    }

    out.addParent(&x);
    if (req) {
        out.setBackwardFunc(reluBackward);
    }
    return out;
}

static void sigmoidBackward(Tensor* self) {
    Tensor* x = self->getParent(0);

    for (int i = 0; i < self->rowCount(); i++) {
        for (int j = 0; j < self->colCount(); j++) {
            double y = self->at(i, j);
            double upstream = self->getGrad(i * self->colCount() + j);
            if (x && x->needGrad()) {
                x->addGrad(i * x->colCount() + j, upstream * y * (1.0 - y));
            }
        }
    }
}

Tensor sigmoid(Tensor& x) {
    bool req = x.needGrad();
    Tensor out(x.rowCount(), x.colCount(), 0.0, req);

    for (int i = 0; i < x.rowCount(); i++) {
        for (int j = 0; j < x.colCount(); j++) {
            out.at(i, j) = 1.0 / (1.0 + std::exp(-x.at(i, j)));
        }
    }

    out.addParent(&x);
    if (req) {
        out.setBackwardFunc(sigmoidBackward);
    }
    return out;
}
