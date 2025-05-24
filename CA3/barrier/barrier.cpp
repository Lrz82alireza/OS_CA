#include "barrier.hpp"

SimpleBarrier barrier;

void SimpleBarrier::init(int expected) {
    expected_total = expected;
    counter = 0;
}

void SimpleBarrier::arrive() {
    if (++counter == expected_total) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_one();
    }
}

void SimpleBarrier::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&] {
        return counter == expected_total;
    });
}
