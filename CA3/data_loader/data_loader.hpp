// data_loader.hpp
#ifndef DATA_LOADER_HPP
#define DATA_LOADER_HPP

#include <stdio.h>
#include <stdint.h>
#include <string>

#define MNIST_IMG_WIDTH 28
#define MNIST_IMG_HEIGHT 28
#define MNIST_IMG_SIZE (MNIST_IMG_WIDTH * MNIST_IMG_HEIGHT)

typedef struct {
    uint8_t pixel[MNIST_IMG_SIZE];
} MNIST_Image;

typedef uint8_t MNIST_Label;

class DataLoader {
private:
    FILE* imageFile;
    FILE* labelFile;

    static uint32_t flipBytes(uint32_t n);
    void readImageFileHeader();
    void readLabelFileHeader();

public:
    DataLoader(const std::string& imagePath, const std::string& labelPath);
    ~DataLoader();

    bool getNext(MNIST_Image& img, MNIST_Label& lbl);
};

#endif
