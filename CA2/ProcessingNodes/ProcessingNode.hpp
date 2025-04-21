#ifndef PROCESSINGNODE_HPP
#define PROCESSINGNODE_HPP

#include "Shared.hpp"

class ProcessingNode
{
private:
    pid_t pid;
public:
    int run();

    ProcessingNode(pid_t pid);
    ~ProcessingNode();
};

inline ProcessingNode::ProcessingNode(pid_t pid) :
    pid(pid)
{
}

inline ProcessingNode::~ProcessingNode()
{
}

#endif // PROCESSINGNODE_HPP