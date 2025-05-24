#include "output_layer_job.hpp"

void enqueue_output_jobs(ThreadPool& pool) {
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        pool.enqueue([=]() {
            double sum = 0.0;
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                sum += hidden_layer[j].output * output_layer[i].weights[j];
            }
            sum += output_layer[i].bias;
            output_layer[i].output = sigmoid(sum);
        });
    }
}
