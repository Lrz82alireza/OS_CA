#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "Shared.hpp"

class Output
{
private:
    string pipePath;
    int numProcesses;

    vector<ScaledData> dataList; // <title, criterion>

public:

    int receiveDataFromProcessingNodes();

    Output(string pipePath, int numProcesses);
    ~Output();
};

inline Output::Output(string pipePath, int numProcesses) : pipePath(pipePath) 
{
    this->numProcesses = numProcesses;
}

inline Output::~Output()
{
}

#endif /* OUTPUT_HPP */