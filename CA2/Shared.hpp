#ifndef SHARED_HPP
#define SHARED_HPP

#include <iostream>      // cout و cin
#include <unistd.h>      // fork(), pipe(), read(), write(), close(), _exit()
#include <sys/wait.h>    // waitpid()
#include <cstring>       // strcpy, strlen و توابع مشابه
#include <cstdlib>       // exit()
#include <sys/types.h>  // برای pid_t
#include <sys/stat.h>   // mkfifo()
#include <fcntl.h>      // open()
#include <string>        
#include <vector>       
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

#define PROC_NUM 3

#define READ_END 0
#define WRITE_END 1

#define FIELD_SIZE 1024

#define NAMED_PIPE_PATH "./NP/pipe"

#define LOADER_PROCESSOR_PIPE_PATH "./pipes/procinfo_pipe"

struct ScaledData
{
    char title[FIELD_SIZE];
    float criterion;
};

struct ProcInfo {
    pid_t pid;
    double cpuUsage;
    char pipePath[FIELD_SIZE];
};

struct ExtractedData
{
    char title[FIELD_SIZE];
    char originalPrice[FIELD_SIZE];
    char discountedPrice[FIELD_SIZE];
    char link[FIELD_SIZE];
    char gameDescription[FIELD_SIZE];
    char recentReviewsSummary[FIELD_SIZE];
    char allReviewsSummary[FIELD_SIZE];
    char recentReviewsNumber[FIELD_SIZE];
    char allReviewsNumber[FIELD_SIZE];
    char developer[FIELD_SIZE];
    char publisher[FIELD_SIZE];
    char popularTags[FIELD_SIZE];
    char gameFeatures[FIELD_SIZE];
    char minimumRequirements[FIELD_SIZE];
};

struct TransformerData
{
    char title[FIELD_SIZE];
    float originalPrice;
    float discountedPrice;
    float recentReviewsSummary;
    float allReviewsSummary;
    float recentReviewsNumber;
    float allReviewsNumber;
};

inline map<string, int> ReviewsSummaryMap = {
    {"Overwhelmingly Positive", 7},
    {"Very Positive", 6},
    {"Positive", 5},
    {"Mostly Positive", 4},
    {"Mixed", 3},
    {"Mostly Negative", 2},
    {"Overwhelmingly Negative", 1}
};

string substr_(const char* src, int start, int length);
void printData(const ExtractedData& data);
void printData(const TransformerData& data);
void printData(const ProcInfo& data);
void safeCopy(char* dest, const string& src);
void printDataList(const vector<TransformerData>& dataList);
void printDataList(const vector<ProcInfo>& procInfos);

#endif // SHARED_HPP