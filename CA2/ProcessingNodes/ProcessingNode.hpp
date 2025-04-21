#ifndef PROCESSINGNODE_HPP
#define PROCESSINGNODE_HPP

#include "Shared.hpp"

class ProcessingNode
{
private:
    int id;
    string pipePath;

    TransformerData minData;
    TransformerData maxData;
    vector<TransformerData> dataList;

    vector<ScaledData> scaledDataList;
    
public:
    void listenAndCompute();

    int receiveDataFromLoader();
    int receiveMinMaxFromLoader();

    // Compute
    int computeScaledDataList();
    float computeScaled(float value, float min, float max);
    ScaledData computeScaledData(const TransformerData& data);
    float computeCriterion(const TransformerData& scaledData);


    ProcessingNode(int id, string pipePath);
    ~ProcessingNode();
};

inline ProcessingNode::ProcessingNode(int id, string pipePath) :
    id(id), pipePath(pipePath)
{
}

inline ProcessingNode::~ProcessingNode()
{
    
}

#endif // PROCESSINGNODE_HPP