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
    int sendDataToLoader(const std::vector<TransformerData>& dataList);

    float priceToPercent(float price, float discount);
    int extractReviewsNumber(const string& str);
public:
    Transformer(pid_t pid, int fd[2]);
    ~Transformer();

    int run();
};

inline Transformer::Transformer(pid_t pid, int fd[2])
                : pid(pid), fd{fd[0], fd[1]}
{
}

inline Transformer::~Transformer()
{
    if (pid == 0)
    {
        _exit(0);
    }
}

#endif // TRANSFORMER_HPP