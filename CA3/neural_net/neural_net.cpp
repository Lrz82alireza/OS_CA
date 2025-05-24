#include "neural_net.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

int INPUT_SIZE = 784;
int HIDDEN_SIZE = 256;
int OUTPUT_SIZE = 10;

std::vector<Hidden_Node> hidden_layer(HIDDEN_SIZE);
std::vector<Output_Node> output_layer(OUTPUT_SIZE);

void loadHiddenLayerParams(const std::string& weightsFile, const std::string& biasFile) {
    std::ifstream weights(weightsFile);
    std::ifstream biases(biasFile);

    if (!weights || !biases) {
        std::cerr << "Failed to open hidden layer param files\n";
        std::exit(1);
    }

    int idx = 0;
    std::string line;

    while (std::getline(weights, line) && idx < HIDDEN_SIZE) {
        std::stringstream ss(line);
        for (int j = 0; j < INPUT_SIZE; ++j)
            ss >> hidden_layer[idx].weights[j];
        idx++;
    }
    weights.close();

    idx = 0;
    while (std::getline(biases, line) && idx < HIDDEN_SIZE) {
        std::stringstream ss(line);
        ss >> hidden_layer[idx].bias;
        idx++;
    }
    biases.close();
}

void loadOutputLayerParams(const std::string& weightsFile, const std::string& biasFile) {
    std::ifstream weights(weightsFile);
    std::ifstream biases(biasFile);

    if (!weights || !biases) {
        std::cerr << "Failed to open output layer param files\n";
        std::exit(1);
    }

    int idx = 0;
    std::string line;

    while (std::getline(weights, line) && idx < OUTPUT_SIZE) {
        std::stringstream ss(line);
        for (int j = 0; j < HIDDEN_SIZE; ++j)
            ss >> output_layer[idx].weights[j];
        idx++;
    }
    weights.close();

    idx = 0;
    while (std::getline(biases, line) && idx < OUTPUT_SIZE) {
        std::stringstream ss(line);
        ss >> output_layer[idx].bias;
        idx++;
    }
    biases.close();
}



double relu(double x) {
    return x > 0 ? x : 0;
}

double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

int predict() {
    double max_out = output_layer[0].output;
    int max_index = 0;

    for (int i = 1; i < OUTPUT_SIZE; ++i) {
        if (output_layer[i].output > max_out) {
            max_out = output_layer[i].output;
            max_index = i;
        }
    }

    return max_index;
}

void initializeNetworkStructure() {
    hidden_layer.resize(HIDDEN_SIZE);
    output_layer.resize(OUTPUT_SIZE);

    for (auto& h : hidden_layer) {
        h.weights.resize(INPUT_SIZE);
    }

    for (auto& o : output_layer) {
        o.weights.resize(HIDDEN_SIZE);
    }
}
