#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "Shared.hpp"

#define GAME_RANKING_FILE_PATH "./GameRanking.csv"

class Output
{
private:
    string pipePath;
    int numProcesses;

    vector<ScaledData> dataList; // <title, criterion>

public:

    int receiveDataFromProcessingNodes();

    int makeOutputFile(const string &filePath);
    void sortDataList();

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