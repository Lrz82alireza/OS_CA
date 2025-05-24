#include "shared_queue.hpp"
#include "reader_thread.hpp"
#include "neural_net.hpp"
#include "thread_pool.hpp"
#include "output_layer_job.hpp"
#include "hidden_layer_job.hpp"

#include <iostream>
#include <thread>

// پیکربندی تست
constexpr int NUM_THREADS = 8;
constexpr int NEURONS_PER_JOB = 32;

int main(int argc, char* argv[]) {
    if (argc >= 2) HIDDEN_SIZE = std::stoi(argv[1]);
    if (argc >= 3) OUTPUT_SIZE = std::stoi(argv[2]);
    
    initializeNetworkStructure();
    std::cout << "Using HIDDEN_SIZE = " << HIDDEN_SIZE << ", OUTPUT_SIZE = " << OUTPUT_SIZE << std::endl;

    // 1. بارگذاری وزن‌ها و بایاس‌ها
    loadHiddenLayerParams("net_params/hidden_weights.txt", "net_params/hidden_biases.txt");
    loadOutputLayerParams("net_params/out_weights.txt", "net_params/out_biases.txt");

    // 2. صف اشتراکی بین reader و main
    SharedQueue<std::pair<MNIST_Image, MNIST_Label>> input_queue;

    // 3. اجرای نخ reader برای بارگذاری داده‌ها
    std::thread reader(reader_thread, std::ref(input_queue));
    reader.join(); // صبر کن تا داده‌ها آماده بشن

    // 4. گرفتن فقط یک تصویر از صف برای تست
    auto [img, lbl] = input_queue.pop();

    // 5. ساخت ThreadPool
    ThreadPool pool(NUM_THREADS);

    // 6. پردازش لایه‌ی پنهان
    enqueue_hidden_jobs(pool, img, HIDDEN_SIZE, NEURONS_PER_JOB);

    // 7. پردازش لایه‌ی خروجی
    enqueue_output_jobs(pool);

    // ❗ تا اینجا فقط job ها enqueue شدن، برای اطمینان از پایان اجرا:
    // به شکل ساده: یه تاخیر کوتاه یا قفل یا شرط می‌ذاریم — اینجا فرض می‌گیریم پردازش تموم شده
    // بهتره در آینده Barrier اضافه شه

    // 8. پیش‌بینی و مقایسه
    int prediction = predict();
    std::cout << "Prediction: " << prediction << " | Actual: " << (int)lbl << std::endl;

    if (prediction == lbl)
        std::cout << "✅ Correct" << std::endl;
    else
        std::cout << "❌ Incorrect" << std::endl;

    return 0;
}
