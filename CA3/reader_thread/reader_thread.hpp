#ifndef READER_THREAD_HPP
#define READER_THREAD_HPP

#include "data_loader.hpp"
#include "shared_queue.hpp"
#include <utility>

const int TOTAL_IMAGES = 10000;

void reader_thread(SharedQueue<std::pair<MNIST_Image, MNIST_Label>>& q);

#endif
