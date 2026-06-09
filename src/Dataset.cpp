#include"Dataset.h"
#include<cstdlib>
#include<cmath>
#include<random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double uniform(double min, double max) {
    double t = rand() / double(RAND_MAX);
    return min + t * (max - min);
}
void generateConcentricCircles(Tensor& x, Tensor& y, int numSamples, double radiusInner, double radiusOuter, double noise) {
    if (numSamples <= 0)return;
    
	x = Tensor(numSamples, 2, 0.0, false); 
	y = Tensor(numSamples, 1, 0.0, false);
    std::default_random_engine gen(42);
    std::normal_distribution<double> norm(0.0, noise);

    int half = numSamples / 2;
    for (int i = 0; i < numSamples; i++) {
        double r, theta, label;
        // 前half个样本是类别0（内圆），后一半是类别1（外环）
        if (i < half) {
            r = uniform(0.0, radiusInner);
            label = 0.0;
        }
        else {
            r = uniform(radiusInner, radiusOuter);
            label = 1.0;
        }
        theta = uniform(0.0, 2.0 * M_PI);
        double px = r * cos(theta);
        double py = r * sin(theta);
        if (noise > 0.0) {
            px += norm(gen);
            py += norm(gen);
        }
        x.at(i, 0) = px;
        x.at(i, 1) = py;
        y.at(i, 0) = label;
    }
}