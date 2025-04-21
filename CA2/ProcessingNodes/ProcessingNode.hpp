#ifndef PROCESSINGNODE_HPP
#define PROCESSINGNODE_HPP

#include "Shared.hpp"

class ProcessingNode
{
private:
    int id;
    string pipePath;
public:
    int run();

    void listenAndCompute();
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