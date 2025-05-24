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
    std::ifstream wfile(weightsFile);
    std::ifstream bfile(biasFile);

    if (!wfile || !bfile) {
        std::cerr << "Failed to open hidden layer param files\n";
        exit(1);
    }

    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        for (int j = 0; j < INPUT_SIZE; ++j) {
            wfile >> hidden_layer[i].weights[j];
        }
    }

    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        bfile >> hidden_layer[i].bias;
    }
}

void loadOutputLayerParams(const std::string& weightsFile, const std::string& biasFile) {
    std::ifstream wfile(weightsFile);
    std::ifstream bfile(biasFile);

    if (!wfile || !bfile) {
        std::cerr << "Failed to open output layer param files\n";
        exit(1);
    }

    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        for (int j = 0; j < HIDDEN_SIZE; ++j) {
            wfile >> output_layer[i].weights[j];
        }
    }

    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        bfile >> output_layer[i].bias;
    }
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
