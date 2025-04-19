#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include "Shared.hpp"

class Transformer
{
private:
    pid_t pid; 
    int fd[2];

    vector<TransformerData> transformedDataList;

    vector<ExtractedData> readDataFromExtractor();
    TransformerData transformLine(const ExtractedData& data);
    vector<TransformerData> transform(const vector<ExtractedData>& dataList);
    int sendDataToLoader(vector<TransformerData> dataList);

    float priceToPercent(float price, float discount);
    int extractReviewsNumber(const string& str);
public:
    Transformer(pid_t pid, int fd[2]);
    ~Transformer();

    int run();
};

Transformer::Transformer(pid_t pid, int fd[2])
                : pid(pid), fd{fd[0], fd[1]}
{
}

Transformer::~Transformer()
{
}

#endif // TRANSFORMER_HPP