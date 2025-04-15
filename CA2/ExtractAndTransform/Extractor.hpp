#ifndef ETRACTOR_HPP
#define ETRACTOR_HPP

#include "Shared.hpp"

class Extractor
{
private:
    pid_t pid;
    int fd[2];

    ExtractedData extractLine(const string& line);
public:
    Extractor(pid_t pid, int fd[2]);
    ~Extractor();
    
    vector<ExtractedData> extract(string path);
    int sendDataToTransformer(vector<ExtractedData> dataList);
};

inline Extractor::Extractor(pid_t pid, int fd[2])
                : pid(pid), fd{fd[0], fd[1]}
{
}

Extractor::~Extractor()
{
}

#endif // ETRACTOR_HPP