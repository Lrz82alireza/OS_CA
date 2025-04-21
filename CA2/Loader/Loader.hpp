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

    vector<vector<TransformerData>> chunkedData;

    vector<ProcInfo> procInfos;

    void processMinMax(const TransformerData &item);
public:
    int receiveDataFromTransformer();
    int receiveProcInfoFromProcessing(const string &pipePath);

    void chunkData();
    void sendDataToProcessingNodes();
    void sendDataToProcessingNode(const vector<TransformerData> &data, const string &pipePath);
    // void sendMinMaxToProcessingNode(const TransformerData &minData, const TransformerData &maxData, const string &pipePath);

    void killProcessingNodes();

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