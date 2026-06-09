#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include <string>
#include <vector>

#include "Loss.h"
#include "Module.h"
#include "Optimizer.h"
#include "Tensor.h"

enum ActivationType {
    ACT_NONE,
    ACT_RELU,
    ACT_SIGMOID
};

class Sequential {
private:
    struct LayerConfig {
        Linear* layer;
        ActivationType activation;
    };

    std::vector<LayerConfig> layers;
    std::vector<Tensor*> cache;
    SGD* optimizer;

    void clearCache();

public:
    Sequential();
    ~Sequential();

    Sequential(const Sequential& other) = delete;
    Sequential& operator=(const Sequential& other) = delete;

    void add(Linear& layer, ActivationType activation = ACT_NONE);
    void registerParameters(SGD& optimizer) const;
    void compile(SGD& optimizer);

    Tensor& forward(Tensor& x);
    TrainResult fit(Tensor& x, Tensor& target, int epochs, int logInterval = 0);

    void saveParameters(const std::string& filename) const;
    int layerCount() const;
};

#endif
