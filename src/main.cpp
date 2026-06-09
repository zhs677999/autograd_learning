#include <iostream>
#include <cstring>
#include <iomanip>
#include <cmath>

#include "Tensor.h"
#include "Ops.h"
#include "Module.h"
#include "Optimizer.h"
#include "Loss.h"
#include "Sequential.h"

using namespace std;

void runBasic() {
    Tensor x(2.0);
    Tensor w(3.0);
    Tensor b(1.0);
    Tensor target(10.0);

    Tensor wx = w * x;
    Tensor y = wx + b;
    Tensor diff = y - target;
    Tensor loss = diff * diff;

    loss.backward();

    cout << "===== Basic AutoGrad Demo =====" << endl;
    cout << "loss = " << loss.value() << endl;
    cout << "grad of w = " << w.gradValue() << endl;
    cout << "grad of b = " << b.gradValue() << endl;
}

void runStandard() {
    cout << "===== Standard Linear Regression Demo =====" << endl;
    cout << "TODO: train y = w1*x1 + w2*x2 + b" << endl;

    // 1. 准备训练数据
    const double true_w1 = 2.5;
    const double true_w2 = -1.2;
    const double true_b = 0.5;
    const int num_samples = 200;

    Tensor X(num_samples, 2, 0.0, false);        // 输入特征，不需要梯度
    Tensor y_true(num_samples, 1, 0.0, false);   // 真实标签，不需要梯度

    srand(42);
    for (int i = 0; i < num_samples; ++i) {
        double x1 = (rand() % 100) / 100.0;
        double x2 = (rand() % 100) / 100.0;
        X.at(i, 0) = x1;
        X.at(i, 1) = x2;
        y_true.at(i, 0) = true_w1 * x1 + true_w2 * x2 + true_b;
    }

    Linear layer(2, 1);
    Tensor& W = layer.weight();    // 权重 [2,1]，需要梯度
    Tensor& b = layer.bias();      // 偏置 [1,1]，需要梯度

    Sequential model;
    model.add(layer);

    SGD optimizer(0.05);
    model.compile(optimizer);

    model.fit(X, y_true, 5000, 50);
    model.saveParameters("output/standard_params.txt");

    cout << "训练完成" << endl;
    // 5. 验证部分
    cout << "\n===== 验证部分（仅用于调试）=====" << endl;
    cout << "训练得到的参数：" << endl;
    cout << "w1 = " << W.at(0, 0) << ", w2 = " << W.at(1, 0) << ", b = " << b.at(0, 0) << endl;
    cout << "真实参数：" << endl;
    cout << "w1 = " << true_w1 << ", w2 = " << true_w2 << ", b = " << true_b << endl;

    double err_w1 = abs(W.at(0, 0) - true_w1) / abs(true_w1);
    double err_w2 = abs(W.at(1, 0) - true_w2) / abs(true_w2);
    double err_b = abs(b.at(0, 0) - true_b) / abs(true_b);

    cout << "相对误差：w1=" << err_w1 * 100 << "%, w2=" << err_w2 * 100 << "%, b=" << err_b * 100 << "%" << endl;

    if (err_w1 < 0.05 && err_w2 < 0.05 && err_b < 0.05) {
        cout << "验证通过：梯度下降正确实现了参数收敛。" << endl;
    }
    else {
        cout << "验证失败：参数未收敛，请检查学习率或训练轮数。" << endl;
    }
    cout << "=============================================" << endl;
}

void runChallenge() {
    cout << "===== Challenge XOR Neural Network Demo =====" << endl;

    double xData[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0
    };
    double yData[] = {
        0.0,
        1.0,
        1.0,
        0.0
    };

    Tensor x(xData, 4, 2, false);
    Tensor target(yData, 4, 1, false);

    Linear layer1(2, 4);
    Linear layer2(4, 1);

    Sequential model;
    model.add(layer1, ACT_SIGMOID);
    model.add(layer2, ACT_SIGMOID);

    SGD optimizer(0.5);
    model.compile(optimizer);

    trainLoss(layer1, layer2, x, target, optimizer, 20000, 2000);

    Tensor& pred = model.forward(x);

    cout << "XOR predictions:" << endl;
    for (int i = 0; i < pred.rowCount(); i++) {
        int predictedClass = pred.at(i, 0) >= 0.5 ? 1 : 0;
        cout << static_cast<int>(x.at(i, 0)) << " "
             << static_cast<int>(x.at(i, 1)) << " -> "
             << fixed << setprecision(4) << pred.at(i, 0)
             << " -> class " << predictedClass << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: tinytensor.exe [basic | standard | challenge | all]" << endl;
        return 0;
    }

    string mode = argv[1];

    if (mode == "basic") {
        runBasic();
    } else if (mode == "standard") {
        runStandard();
    } else if (mode == "challenge") {
        runChallenge();
    } else if (mode == "all") {
        runBasic();
        runStandard();
        runChallenge();
    } else {
        cout << "Unknown mode." << endl;
    }

    return 0;
}
