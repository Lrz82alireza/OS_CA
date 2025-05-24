#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <mutex>
#include <condition_variable>
#include <atomic>


class SimpleBarrier {
    public:
    void init(int expected);
    void arrive();
    void wait();
    
    private:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<int> counter = 0;
    int expected_total = 0;
};

extern SimpleBarrier barrier;

#endif
