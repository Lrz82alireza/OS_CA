#ifndef LOADER_HPP
#define LOADER_HPP

#include "Shared.hpp"

class Loader
{
private:
    
    string path;
    vector<TransformerData> dataList;

    int receiveDataFromTransformer();
public:
    Loader(const string &path);
    ~Loader();
};


inline Loader::Loader(const string &path) : path(path)
{
}

inline Loader::~Loader()
{
}

#endif // LOADER_HPP