#ifndef NEURAL_NET_HPP
#define NEURAL_NET_HPP

#include <vector>
#include <string>

extern int INPUT_SIZE;
extern int HIDDEN_SIZE;
extern int OUTPUT_SIZE;

struct Hidden_Node {
    std::vector<double> weights;
    double bias;
    double output;
};

struct Output_Node {
    std::vector<double> weights;
    double bias;
    double output;
};

// Global layer containers
extern int NUM_HIDDEN_LAYERS;
extern std::vector<std::vector<Hidden_Node>> hidden_layers;

extern std::vector<Output_Node> output_layer;

// Loaders
void loadHiddenLayerParams(const std::string& weightsFile, const std::string& biasFile);
void loadOutputLayerParams(const std::string& weightsFile, const std::string& biasFile);

// Activation
double relu(double x);
double sigmoid(double x);

// Predict Output
int predict();

void initializeNetworkStructure();

#endif
