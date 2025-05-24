#include "shared_queue.hpp"
#include "reader_thread.hpp"
#include "neural_net.hpp"
#include "thread_pool.hpp"
#include "output_layer_job.hpp"
#include "hidden_layer_job.hpp"
#include "barrier.hpp"

#include <iostream>
#include <thread>

constexpr int NUM_THREADS = 8;
constexpr int NEURONS_PER_JOB = 32;

void printProgress(int current, int total, int barWidth = 40) {
    float progress = float(current) / total;
    int pos = barWidth * progress;

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "█";
        else std::cout << "-";
    }
    std::cout << "] " << current << " / " << total << " (" << int(progress * 100.0) << "%)";
    std::cout.flush();
}

double process_dataset(
    ThreadPool& pool,
    SharedQueue<std::pair<MNIST_Image, MNIST_Label>>& input_queue,
    int neurons_per_job)
{
    int correct = 0;
    int total = 0;

    while (!input_queue.empty()) {
        auto [img, lbl] = input_queue.pop();
        total++;

        int expected_hidden_jobs = (HIDDEN_SIZE + neurons_per_job - 1) / neurons_per_job;
        barrier.init(expected_hidden_jobs);

        enqueue_hidden_jobs(pool, img, HIDDEN_SIZE, neurons_per_job);
        barrier.wait();
        enqueue_output_jobs(pool);

        
        int prediction = predict();
        bool is_correct = (prediction == lbl);
        if (is_correct) correct++;
        
        std::cout << "\n[Image #" << total << "] "
                  << "Prediction: " << prediction
                  << " | Actual: " << (int)lbl
                  << " --> " << (is_correct ? "✅ Correct" : "❌ Incorrect") << std::endl;
        
        printProgress(total, TOTAL_IMAGES);        
    }

    std::cout << std::endl; // خط جدید بعد از progress bar
    return 100.0 * correct / total;
}

int main(int argc, char* argv[]) {
    if (argc >= 2) HIDDEN_SIZE = std::stoi(argv[1]);

    initializeNetworkStructure();
    std::cout << "Using HIDDEN_SIZE = " << HIDDEN_SIZE << std::endl;

    // Load network parameters
    loadHiddenLayerParams("net_params/hidden_weights.txt", "net_params/hidden_biases.txt");
    loadOutputLayerParams("net_params/out_weights.txt", "net_params/out_biases.txt");

    // Reader + queue setup
    SharedQueue<std::pair<MNIST_Image, MNIST_Label>> input_queue;
    std::thread reader(reader_thread, std::ref(input_queue));
    reader.join();

    // Build thread pool
    ThreadPool pool(NUM_THREADS);

    // Run full dataset
    double acc = process_dataset(pool, input_queue, NEURONS_PER_JOB);
    std::cout << "\n✅ Final Accuracy: " << acc << "%\n";

    return 0;
}
