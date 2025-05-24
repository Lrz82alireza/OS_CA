#include "hidden_layer_job.hpp"
#include "neural_net.hpp"
#include "barrier.hpp"
#include <iostream>

void enqueue_all_hidden_layers(
    ThreadPool& pool,
    const MNIST_Image& img,
    int neurons_per_job)
{
    std::vector<double> input_buffer(INPUT_SIZE);
    for (int j = 0; j < INPUT_SIZE; ++j)
        input_buffer[j] = img.pixel[j];

    for (int l = 0; l < NUM_HIDDEN_LAYERS; ++l) {
        const std::vector<double>& input = (l == 0)
            ? input_buffer
            : [&]() -> const std::vector<double>& {
                static std::vector<double> temp(HIDDEN_SIZE);
                for (int i = 0; i < HIDDEN_SIZE; ++i)
                    temp[i] = hidden_layers[l - 1][i].output;
                return temp;
            }();

        int num_jobs = (HIDDEN_SIZE + neurons_per_job - 1) / neurons_per_job;
        barrier.init(num_jobs);

        for (int start = 0; start < HIDDEN_SIZE; start += neurons_per_job) {
            int end = std::min(start + neurons_per_job, HIDDEN_SIZE);

            pool.enqueue([start, end, l, input](void) {
                for (int i = start; i < end; ++i) {
                    double sum = 0.0;

                    // Validate input/weight size match
                    if (hidden_layers[l][i].weights.size() != input.size()) {
                        std::cerr << "[ERROR] Size mismatch: input=" << input.size()
                                  << " vs weights=" << hidden_layers[l][i].weights.size()
                                  << " at layer=" << l << ", neuron=" << i << std::endl;
                        std::exit(1);
                    }

                    for (size_t j = 0; j < input.size(); ++j)
                        sum += input[j] * hidden_layers[l][i].weights[j];

                    sum += hidden_layers[l][i].bias;
                    hidden_layers[l][i].output = relu(sum);
                }
                barrier.arrive();
            });
        }

        barrier.wait();
    }
}
