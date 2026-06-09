#include "Sequential.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "Ops.h"

static const char* activationName(ActivationType activation) {
    switch (activation) {
    case ACT_NONE:
        return "none";
    case ACT_RELU:
        return "relu";
    case ACT_SIGMOID:
        return "sigmoid";
    default:
        return "unknown";
    }
}

static void printSequentialLoss(int epoch, double value) {
    std::cout << "epoch " << std::setw(5) << epoch
              << " loss = " << std::fixed << std::setprecision(6)
              << value << std::endl;
}

Sequential::Sequential() : optimizer(nullptr) {}

Sequential::~Sequential() {
    clearCache();
}

void Sequential::clearCache() {
    for (int i = 0; i < static_cast<int>(cache.size()); i++) {
        delete cache[i];
    }
    cache.clear();
}

void Sequential::add(Linear& layer, ActivationType activation) {
    LayerConfig config;
    config.layer = &layer;
    config.activation = activation;
    layers.push_back(config);
}

void Sequential::registerParameters(SGD& optimizer) const {
    for (int i = 0; i < static_cast<int>(layers.size()); i++) {
        if (!layers[i].layer) {
            continue;
        }
        optimizer.addParam(&layers[i].layer->weight());
        optimizer.addParam(&layers[i].layer->bias());
    }
}

void Sequential::compile(SGD& optimizer) {
    this->optimizer = &optimizer;
    registerParameters(optimizer);
}

Tensor& Sequential::forward(Tensor& x) {
    if (layers.empty()) {
        throw std::logic_error("Sequential has no layers.");
    }

    clearCache();

    Tensor* current = &x;
    for (int i = 0; i < static_cast<int>(layers.size()); i++) {
        if (!layers[i].layer) {
            throw std::logic_error("Sequential contains an empty layer.");
        }

        Tensor linearOut = layers[i].layer->forward(*current);
        Tensor* savedLinear = new Tensor(linearOut);
        cache.push_back(savedLinear);
        current = savedLinear;

        if (layers[i].activation == ACT_RELU) {
            Tensor activated = relu(*current);
            Tensor* savedActivation = new Tensor(activated);
            cache.push_back(savedActivation);
            current = savedActivation;
        } else if (layers[i].activation == ACT_SIGMOID) {
            Tensor activated = sigmoid(*current);
            Tensor* savedActivation = new Tensor(activated);
            cache.push_back(savedActivation);
            current = savedActivation;
        } else if (layers[i].activation != ACT_NONE) {
            throw std::invalid_argument("Unsupported activation type.");
        }
    }

    return *current;
}

TrainResult Sequential::fit(Tensor& x, Tensor& target, int epochs, int logInterval) {
    if (!optimizer) {
        throw std::logic_error("Call compile() before fit().");
    }
    if (epochs <= 0) {
        throw std::invalid_argument("epochs must be positive.");
    }

    TrainResult result;
    result.finalLoss = 0.0;
    result.epochs = epochs;

    for (int epoch = 1; epoch <= epochs; epoch++) {
        optimizer->zeroGrad();

        Tensor& pred = forward(x);
        Tensor loss = mseLoss(pred, target);
        result.finalLoss = loss.value();

        loss.backward();
        optimizer->step();

        if (logInterval > 0 && (epoch % logInterval == 0 || epoch == 1)) {
            printSequentialLoss(epoch, result.finalLoss);
        }

        clearCache();
    }

    return result;
}

void Sequential::saveParameters(const std::string& filename) const {
    std::ofstream fout(filename.c_str());
    if (!fout.is_open()) {
        throw std::runtime_error("Cannot open parameter file.");
    }

    fout << std::setprecision(17);
    fout << "TinyTensor Sequential Parameters" << std::endl;
    fout << "layers " << layers.size() << std::endl;

    for (int i = 0; i < static_cast<int>(layers.size()); i++) {
        Linear* layer = layers[i].layer;
        if (!layer) {
            continue;
        }

        Tensor& W = layer->weight();
        Tensor& b = layer->bias();

        fout << "layer " << i << " Linear activation "
             << activationName(layers[i].activation) << std::endl;

        fout << "weight " << W.rowCount() << " " << W.colCount() << std::endl;
        for (int r = 0; r < W.rowCount(); r++) {
            for (int c = 0; c < W.colCount(); c++) {
                fout << W.at(r, c);
                if (c < W.colCount() - 1) {
                    fout << " ";
                }
            }
            fout << std::endl;
        }

        fout << "bias " << b.rowCount() << " " << b.colCount() << std::endl;
        for (int r = 0; r < b.rowCount(); r++) {
            for (int c = 0; c < b.colCount(); c++) {
                fout << b.at(r, c);
                if (c < b.colCount() - 1) {
                    fout << " ";
                }
            }
            fout << std::endl;
        }
    }
}

int Sequential::layerCount() const {
    return static_cast<int>(layers.size());
}
