#include "Ops.h"
#include <stdexcept>

//加法运算
static void addBackward(Tensor* self) {
    // 获取前向时记录的父节点
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);
    // 获取上游传下来的梯度
    double upstream = self->getGrad(0);
    // 加法的局部导数：dz/da = 1, dz/db = 1
    // a.grad += upstream * 1
    if (a && a->needGrad()) {
        a->addGrad(0, upstream);
    }
    if (b && b->needGrad()) {
        b->addGrad(0, upstream);
    }
}

Tensor operator+(Tensor& a, Tensor& b) {
    //前向计算数值并创建新 Tensor
    bool req = a.needGrad() || b.needGrad();
    Tensor out(a.value() + b.value(), req);

    //记录父节点，建立计算图
    out.addParent(&a);
    out.addParent(&b);

    //绑定反向传播函数
    if (req) {
        out.setBackwardFunc(addBackward);
    }
    return out;
}


//减法运算

static void subBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    double upstream = self->getGrad(0);

    // 减法的局部导数：dz/da = 1, dz/db = -1
    if (a && a->needGrad()) {
        a->addGrad(0, upstream);
    }
    if (b && b->needGrad()) {
        b->addGrad(0, -upstream);      // 传给减数的梯度要加负号
    }
}

Tensor operator-(Tensor& a, Tensor& b) {
    bool req = a.needGrad() || b.needGrad();
    Tensor out(a.value() - b.value(), req);

    out.addParent(&a);
    out.addParent(&b);

    if (req) {
        out.setBackwardFunc(subBackward);
    }
    return out;
}


//乘法运算
static void mulBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    double upstream = self->getGrad(0);

    // 乘法的局部导数：dz/da = b, dz/db = a
    if (a && a->needGrad()) {
        a->addGrad(0, upstream * b->value()); 
    }
    if (b && b->needGrad()) {
        b->addGrad(0, upstream * a->value());
    }
}

Tensor operator*(Tensor& a, Tensor& b) {
    bool req = a.needGrad() || b.needGrad();
    Tensor out(a.value() * b.value(), req);

    out.addParent(&a);
    out.addParent(&b);

    if (req) {
        out.setBackwardFunc(mulBackward);
    }
    return out;
}


//除法运算

static void divBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    double upstream = self->getGrad(0);

    // 除法的局部导数：dz/da = 1/b, dz/db = -a/(b^2)
    if (a && a->needGrad()) {
        a->addGrad(0, upstream / b->value());
    }
    if (b && b->needGrad()) {
        double b_val = b->value();
        b->addGrad(0, upstream * (-a->value()) / (b_val * b_val));
    }
}

Tensor operator/(Tensor& a, Tensor& b) {
    if (b.value() == 0.0) {
        throw std::runtime_error("Division by zero in Tensor operation.");
    }
    bool req = a.needGrad() || b.needGrad();
    Tensor out(a.value() / b.value(), req);

    out.addParent(&a);
    out.addParent(&b);

    if (req) {
        out.setBackwardFunc(divBackward);
    }
    return out;
}


//MSE

static void mseBackward(Tensor* self) {
    Tensor* pred = self->getParent(0);
    Tensor* target = self->getParent(1);

    double upstream = self->getGrad(0);

    // 标量 MSE：loss = (pred - target)^2
    // d(loss)/d(pred) = 2 * (pred - target)
    if (pred && pred->needGrad()) {
        double diff = pred->value() - target->value();
        pred->addGrad(0, upstream * 2.0 * diff);
    }
    if (target && target->needGrad()) {
        double diff = pred->value() - target->value();
        target->addGrad(0, upstream * -2.0 * diff);
    }
}

Tensor mseLoss(Tensor& pred, Tensor& target) {
    bool req = pred.needGrad() || target.needGrad();
    double diff = pred.value() - target.value();
    Tensor out(diff * diff, req);
    out.addParent(&pred);
    out.addParent(&target);

    if (req) {
        out.setBackwardFunc(mseBackward);
    }
    return out;
}


//后续继续

Tensor matmul(Tensor& a, Tensor& b) {
    throw std::logic_error("matmul not implemented in basic tier.");
}

Tensor relu(Tensor& x) {
    throw std::logic_error("relu not implemented in basic tier.");
}

Tensor sigmoid(Tensor& x) {
    throw std::logic_error("sigmoid not implemented in basic tier.");
}