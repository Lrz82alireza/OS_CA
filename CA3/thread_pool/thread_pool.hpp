#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <pthread.h>
#include <queue>
#include <functional>
#include <vector>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    void enqueue(const std::function<void()>& task);

private:
    static void* worker(void* arg);

    struct WorkerContext {
        ThreadPool* pool;
        size_t id;
    };

    std::queue<std::function<void()>> tasks;
    pthread_mutex_t queue_mutex;
    pthread_cond_t condition;
    std::vector<pthread_t> threads;
    bool stop;

    friend struct WorkerContext;
};

#endif
