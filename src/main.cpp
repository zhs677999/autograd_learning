#include <iostream>
#include <cstring>
#include "Tensor.h"
#include "Ops.h"
#include "Module.h"
#include "Optimizer.h"

using namespace std;

void runBasic() {
    Tensor x(2.0);
    Tensor w(3.0);
    Tensor b(1.0);
    Tensor target(10.0);

    Tensor y = w * x + b;
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
}

void runChallenge() {
    cout << "===== Challenge XOR Neural Network Demo =====" << endl;
    cout << "TODO: train 2-4-1 network for XOR" << endl;
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
