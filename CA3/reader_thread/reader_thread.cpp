#include "reader_thread.hpp"

void reader_thread(SharedQueue<std::pair<MNIST_Image, MNIST_Label>>& q) {
    DataLoader loader("data/t10k-images-idx3-ubyte", "data/t10k-labels-idx1-ubyte");

    MNIST_Image img;
    MNIST_Label lbl;

    for (int i = 0; i < TOTAL_IMAGES; ++i) {
        if (!loader.getNext(img, lbl)) break;
        q.push(std::make_pair(img, lbl));
    }
}
