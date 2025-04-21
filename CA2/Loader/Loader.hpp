#ifndef LOADER_HPP
#define LOADER_HPP

#include "Shared.hpp"

class Loader
{
private:
    
    string path;
    vector<TransformerData> dataList;
    TransformerData minData;
    TransformerData maxData;


    void processMinMax(const TransformerData &item);
public:
    int receiveDataFromTransformer();

    Loader(const string &path);
    ~Loader();
};


inline Loader::Loader(const string &path) : path(path)
{
}

inline Loader::~Loader()
{
    _exit(0);
}

#endif // LOADER_HPP