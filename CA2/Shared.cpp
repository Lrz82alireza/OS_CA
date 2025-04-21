#include "Shared.hpp"

std::string substr_(const char* src, int start, int length) {
    if (std::string(src).size() == 0) {
        // cout << "src is empty" << endl;
        throw std::invalid_argument("Empty string");
        return "";
    }
    return std::string(src).substr(start, length);
}

void printData(const ExtractedData &data)
{
    cout << "Title: " << data.title << endl;
    cout << "Original Price: " << data.originalPrice << endl;
    cout << "Discounted Price: " << data.discountedPrice << endl;
    cout << "Link: " << data.link << endl;
    cout << "Game Description: " << data.gameDescription << endl;
    cout << "Recent Reviews Summary: " << data.recentReviewsSummary << endl;
    cout << "All Reviews Summary: " << data.allReviewsSummary << endl;
    cout << "Recent Reviews Number: " << data.recentReviewsNumber << endl;
    cout << "All Reviews Number: " << data.allReviewsNumber << endl;
    cout << "Developer: " << data.developer << endl;
    cout << "Publisher: " << data.publisher << endl;
    cout << "Popular Tags: " << data.popularTags << endl;
    cout << "Game Features: " << data.gameFeatures << endl;
    cout << "Minimum Requirements: " << data.minimumRequirements << endl;
    cout << "----------------------------------------" << endl;
}

void printData(const TransformerData &data)
{
    cout << "Title: " << data.title << endl;
    cout << "Original Price: " << data.originalPrice << endl;
    cout << "Discounted Price: " << data.discountedPrice << endl;
    cout << "Recent Reviews Summary: " << data.recentReviewsSummary << endl;
    cout << "All Reviews Summary: " << data.allReviewsSummary << endl;
    cout << "Recent Reviews Number: " << data.recentReviewsNumber << endl;
    cout << "All Reviews Number: " << data.allReviewsNumber << endl;
    cout << "----------------------------------------" << endl;
}

void printData(const ProcInfo &data)
{
    cout << "PID: " << data.pid << endl;
    cout << "CPU Usage: " << data.cpuUsage << endl;
    cout << "Pipe Path: " << data.pipePath << endl;
    cout << "----------------------------------------" << endl;
}

void printData(const ScaledData &data)
{
    cout << "Title: " << data.title << endl;
    cout << "Criterion: " << data.criterion << endl;
    cout << "----------------------------------------" << endl;
}

void safeCopy(char* dest, const string& src) {
    if (src.size() == 0) {
        // cout << "src is empty" << endl;
        throw std::invalid_argument("Title is empty");
        return;
    }
    strncpy(dest, src.c_str(), FIELD_SIZE - 1);
    dest[FIELD_SIZE - 1] = '\0';
}

void printDataList(const vector<TransformerData>& dataList)
{
    for (const auto& data : dataList)
    {
        printData(data);
    }
}

void printDataList(const vector<ProcInfo> &procInfos)
{
    for (const auto& info : procInfos)
    {
        printData(info);
    }
}

void printDataList(const vector<ScaledData>& dataList)
{
    for (const auto& data : dataList)
    {
        printData(data);
    }
}