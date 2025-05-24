#ifndef OUTPUT_LAYER_JOB_HPP
#define OUTPUT_LAYER_JOB_HPP

#include "neural_net.hpp"
#include "thread_pool.hpp"

void enqueue_output_jobs(ThreadPool& pool);

#endif
