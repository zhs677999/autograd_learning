#ifndef TENSOR_H
#define TENSOR_H

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>
#include "LinkedList.h"

using namespace std;

class Tensor;
// 函数指针
typedef void (*BackwardFunc)(Tensor* self);

class Tensor {
private:
    double* data;// 变量值，后面进阶成向量值
    double* grad;// 梯度值
    // 形状信息
    int rows;
    int cols;
    // 父节点列表，记录当前 Tensor 的计算图中哪些 Tensor 是它的父节点
    LinkedList<Tensor*> parents;
    // 反向传播函数指针，用于执行当前 Tensor 的反向传播逻辑，多个版本
    BackwardFunc backwardFunc;

    bool requiresGrad;

public:
    Tensor(){
        data = nullptr;
        grad = nullptr;
        rows = 0;
        cols = 0;
        backwardFunc = nullptr;
        requiresGrad = true;
    };
    Tensor(double value, bool reqGrad = true){
        data = new double[1];
        grad = new double[1];
        data[0] = value;
        grad[0] = 0.0;
        rows = 1;
        cols = 1;
        backwardFunc = nullptr;
        requiresGrad = reqGrad;
    };
    /*
        * 自定义形状并给初始值
        * @param r 行数
        * @param c 列数
        * @param initValue 初始值，默认为 0.0
        * @param reqGrad 是否需要计算梯度，默认为 true
        * 返回一个指定形状的 Tensor，所有元素初始化为 initValue，并根据 reqGrad 设置是否需要计算梯度
    */
    Tensor(int r, int c, double initValue = 0.0, bool reqGrad = true){
        data = new double[r * c];
        grad = new double[r * c];
        for (int i = 0; i < r * c; i++) {
            data[i] = initValue;
            grad[i] = 0.0;
        }
        rows = r;
        cols = c;
        backwardFunc = nullptr;
        requiresGrad = reqGrad;
    };

    Tensor(const Tensor& other){
        rows = other.rows;
        cols = other.cols;
        requiresGrad = other.requiresGrad;
        backwardFunc = other.backwardFunc;
        data = new double[rows * cols];
        grad = new double[rows * cols];
        for (int i = 0; i < rows * cols; i++) {
            data[i] = other.data[i];
            grad[i] = other.grad[i];
        }
        // 复制父节点列表
        for (int i = 0; i < other.parents.parentCount(); i++) {
            parents.add(other.parents.getParent(i));
        }
    };
    Tensor& operator=(const Tensor& other){
        if (this == &other) {
            return *this; // 自我赋值检查
        }
        // 释放当前资源
        delete[] data;
        delete[] grad;

        // 复制新资源
        rows = other.rows;
        cols = other.cols;
        requiresGrad = other.requiresGrad;
        backwardFunc = other.backwardFunc;
        data = new double[rows * cols];
        grad = new double[rows * cols];
        for (int i = 0; i < rows * cols; i++) {
            data[i] = other.data[i];
            grad[i] = other.grad[i];
        }
        // 复制父节点列表
        parents = LinkedList<Tensor*>(); // 清空当前父节点列表
        for (int i = 0; i < other.parents.parentCount(); i++) {
            parents.add(other.parents.getParent(i));
        }
        return *this;
    };
    ~Tensor(){
        delete[] data;
        delete[] grad;
    };

    int rowCount() const{
        return rows;
    };
    int colCount() const{
        return cols;
    };
    int size() const{
        return rows * cols;
    };
    // 获取指定位置的元素值，返回引用以允许修改
    double& at(int r, int c){
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            throw out_of_range("Index out of bounds");
        }
        return data[r * cols + c];
    };
    double at(int r, int c) const{
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            throw out_of_range("Index out of bounds");
        }
        return data[r * cols + c];
    };
    // 获取单元素 Tensor 的值
    double value() const{
        if (rows != 1 || cols != 1) {
            throw logic_error("Tensor is not a single element");
        }
        return data[0];
    };
    double gradValue() const{
        if (rows != 1 || cols != 1) {
            throw logic_error("Tensor is not a single element");
        }
        return grad[0];
    };
    void setGrad(int index, double value){
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        grad[index] = value;
    };
    double getGrad(int index) const{
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        return grad[index];
    };
    void addGrad(int index, double value){
        if (index < 0 || index >= size()) {
            throw out_of_range("Index out of bounds");
        }
        grad[index] += value;
    };

    void zeroGrad(){
        for (int i = 0; i < size(); i++) {
            grad[i] = 0.0;
        }
    };

    void addParent(Tensor* p){
        parents.add(p);
    };
    Tensor* getParent(int i);
    int parentCount() const{
        int count = 0;
        Tensor* current = parents.getHead();
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    };

    void setBackwardFunc(BackwardFunc f){
        backwardFunc = f;
    };
    void backward(){
        if (backwardFunc) {
            backwardFunc(this);
        }
    };
    void _backward(){
        if (backwardFunc) {
            backwardFunc(this);
        }
        // 递归调用父节点的 _backward
        for (int i = 0; i < parents.parentCount(); i++) {
            Tensor* parent = parents.getParent(i);
            if (parent) {
                parent->_backward();
            }
        }
    };

    bool needGrad() const{
        return requiresGrad;
    };

    void print() const{
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
    };
    void printGrad() const{
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
    };

    friend Tensor operator+(Tensor& a, Tensor& b);
    friend Tensor operator-(Tensor& a, Tensor& b);
    friend Tensor operator*(Tensor& a, Tensor& b);
    friend Tensor operator/(Tensor& a, Tensor& b);

    friend Tensor matmul(Tensor& a, Tensor& b);
    friend Tensor relu(Tensor& x);
    friend Tensor sigmoid(Tensor& x);
};

#endif
