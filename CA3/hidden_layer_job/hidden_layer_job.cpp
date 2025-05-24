#include "hidden_layer_job.hpp"

void enqueue_hidden_jobs(
    ThreadPool& pool,
    const MNIST_Image& img,
    int total_neurons,
    int neurons_per_task)
{
    for (int i = 0; i < total_neurons; i += neurons_per_task) {
        int start = i;
        int end = std::min(i + neurons_per_task, total_neurons);

        pool.enqueue([=]() {
            for (int n = start; n < end; ++n) {
                double sum = 0.0;
                for (int j = 0; j < INPUT_SIZE; ++j) {
                    sum += img.pixel[j] * hidden_layer[n].weights[j];
                }
                sum += hidden_layer[n].bias;
                hidden_layer[n].output = relu(sum);
            }
        });
    }
}
