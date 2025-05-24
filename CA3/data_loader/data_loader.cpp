// data_loader.cpp
#include "data_loader.hpp"
#include <stdlib.h>

typedef struct {
    uint32_t magicNumber;
    uint32_t maxImages;
    uint32_t imgWidth;
    uint32_t imgHeight;
} MNIST_ImageFileHeader;

typedef struct {
    uint32_t magicNumber;
    uint32_t maxImages;
} MNIST_LabelFileHeader;

uint32_t DataLoader::flipBytes(uint32_t n) {
    return ((n & 0x000000FF) << 24) |
           ((n & 0x0000FF00) << 8)  |
           ((n & 0x00FF0000) >> 8)  |
           ((n & 0xFF000000) >> 24);
}

void DataLoader::readImageFileHeader() {
    MNIST_ImageFileHeader header;
    fread(&header, sizeof(header), 1, imageFile);
    header.magicNumber = flipBytes(header.magicNumber);
}

void DataLoader::readLabelFileHeader() {
    MNIST_LabelFileHeader header;
    fread(&header, sizeof(header), 1, labelFile);
    header.magicNumber = flipBytes(header.magicNumber);
}

DataLoader::DataLoader(const std::string& imagePath, const std::string& labelPath) {
    imageFile = fopen(imagePath.c_str(), "rb");
    if (!imageFile) {
        fprintf(stderr, "Failed to open image file: %s\n", imagePath.c_str());
        exit(1);
    }
    readImageFileHeader();

    labelFile = fopen(labelPath.c_str(), "rb");
    if (!labelFile) {
        fprintf(stderr, "Failed to open label file: %s\n", labelPath.c_str());
        exit(1);
    }
    readLabelFileHeader();
}

DataLoader::~DataLoader() {
    if (imageFile) fclose(imageFile);
    if (labelFile) fclose(labelFile);
}

bool DataLoader::getNext(MNIST_Image& img, MNIST_Label& lbl) {
    size_t imgResult = fread(&img, sizeof(MNIST_Image), 1, imageFile);
    size_t lblResult = fread(&lbl, sizeof(MNIST_Label), 1, labelFile);
    return (imgResult == 1 && lblResult == 1);
}
