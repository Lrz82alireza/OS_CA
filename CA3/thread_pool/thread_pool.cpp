#include "thread_pool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    for (size_t i = 0; i < num_threads; ++i) {
        auto* ctx = new WorkerContext{this, i};
        pthread_t thread;
        if (pthread_create(&thread, nullptr, worker, ctx) != 0) {
            std::cerr << "Failed to create thread " << i << std::endl;
            delete ctx;
        } else {
            threads.push_back(thread);
        }
    }
}

ThreadPool::~ThreadPool() {
    pthread_mutex_lock(&queue_mutex);
    stop = true;
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_broadcast(&condition);

    for (auto& thread : threads) {
        pthread_join(thread, nullptr);
    }

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&condition);
}

void ThreadPool::enqueue(const std::function<void()>& task) {
    pthread_mutex_lock(&queue_mutex);
    tasks.push(task);
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&condition);
}

void* ThreadPool::worker(void* arg) {
    auto* ctx = static_cast<WorkerContext*>(arg);
    ThreadPool* pool = ctx->pool;
    delete ctx;

    while (true) {
        pthread_mutex_lock(&pool->queue_mutex);
        while (pool->tasks.empty() && !pool->stop) {
            pthread_cond_wait(&pool->condition, &pool->queue_mutex);
        }

        if (pool->stop && pool->tasks.empty()) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }

        auto task = pool->tasks.front();
        pool->tasks.pop();
        pthread_mutex_unlock(&pool->queue_mutex);

        task(); // Execute the job
    }

    return nullptr;
}
