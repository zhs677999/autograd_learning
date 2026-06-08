#include <iostream>
#include <cstring>
#include <iomanip>
#include <cmath>

#include "Tensor.h"
#include "Ops.h"
#include "Module.h"
#include "Optimizer.h"
#include "Loss.h"

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

    // 2. 定义可训练线性层
    Linear layer(2, 1);
    Tensor& W = layer.weight();    // 权重 [2,1]，需要梯度
    Tensor& b = layer.bias();      // 偏置 [1,1]，需要梯度

    // 3. 创建优化器并注册参数
    SGD optimizer(0.05);
    optimizer.addParam(&W);
    optimizer.addParam(&b);

    // 4. 训练循环已封装到 trainLoss
    trainLoss(layer, X, y_true, optimizer, 5000, 50);

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

    SGD optimizer(2.5);
    optimizer.addParam(&layer1.weight());
    optimizer.addParam(&layer1.bias());
    optimizer.addParam(&layer2.weight());
    optimizer.addParam(&layer2.bias());

    trainLoss(layer1, layer2, x, target, optimizer, 1000, 100);

    Tensor h = layer1.forward(x);
    Tensor a = sigmoid(h);
    Tensor z = layer2.forward(a);
    Tensor pred = sigmoid(z);

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
