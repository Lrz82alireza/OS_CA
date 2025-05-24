#ifndef HIDDEN_LAYER_JOB_HPP
#define HIDDEN_LAYER_JOB_HPP

#include "shared_queue.hpp"
#include "neural_net.hpp"
#include "thread_pool.hpp"
#include "barrier.hpp"
#include <data_loader.hpp>

void enqueue_all_hidden_layers(
    ThreadPool& pool,
    const MNIST_Image& img,
    int neurons_per_job);

#endif
