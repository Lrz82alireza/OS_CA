#include "neural_net.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

int INPUT_SIZE = 784;
int HIDDEN_SIZE = 256;
int OUTPUT_SIZE = 10;

int NUM_HIDDEN_LAYERS = 1;
std::vector<std::vector<Hidden_Node>> hidden_layers;

std::vector<Output_Node> output_layer(OUTPUT_SIZE);

void loadHiddenLayerParams(const std::string& weightsFile, const std::string& biasFile) {
    std::ifstream wfile(weightsFile);
    std::ifstream bfile(biasFile);

    if (!wfile || !bfile) {
        std::cerr << "Failed to open hidden layer param files\n";
        exit(1);
    }

    for (int l = 0; l < NUM_HIDDEN_LAYERS; ++l) {
        int input_size = (l == 0) ? INPUT_SIZE : HIDDEN_SIZE;
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < input_size; ++j) {
                wfile >> hidden_layers[l][i].weights[j];
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            bfile >> hidden_layers[l][i].bias;
        }
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
    hidden_layers.clear();
    for (int i = 0; i < NUM_HIDDEN_LAYERS; ++i) {
        std::vector<Hidden_Node> layer(HIDDEN_SIZE);
        int input_dim = (i == 0) ? INPUT_SIZE : HIDDEN_SIZE;
        for (auto& h : layer) {
            h.weights.resize(input_dim);
        }
        hidden_layers.push_back(layer);
    }

    output_layer.resize(OUTPUT_SIZE);
    for (auto& o : output_layer) {
        o.weights.resize(HIDDEN_SIZE);
    }
}

