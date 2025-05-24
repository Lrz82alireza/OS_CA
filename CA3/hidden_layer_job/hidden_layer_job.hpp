#ifndef HIDDEN_LAYER_JOB_HPP
#define HIDDEN_LAYER_JOB_HPP

#include "shared_queue.hpp"
#include "neural_net.hpp"
#include "thread_pool.hpp"
#include <data_loader.hpp>

void enqueue_hidden_jobs(
    ThreadPool& pool,
    const MNIST_Image& img,
    int total_neurons,
    int neurons_per_task);

#endif
