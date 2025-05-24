#include "output_layer_job.hpp"

void enqueue_output_jobs(ThreadPool& pool) {
    const std::vector<Hidden_Node>& last_hidden = hidden_layers.back();

    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        pool.enqueue([=]() {
            double sum = 0.0;
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                sum += last_hidden[j].output * output_layer[i].weights[j];
            }
            sum += output_layer[i].bias;
            output_layer[i].output = sigmoid(sum);
        });
    }
}
