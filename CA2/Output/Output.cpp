#include "Output.hpp"

int Output::receiveDataFromProcessingNodes()
{
    int fd = open(pipePath.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("open failed (output)");
        return -1;
    }

    cout << "OUTPUT OPENED PATH: " << pipePath << endl;

    int endCounter = 0;

    while (true) {
        ScaledData item;
        ssize_t bytesRead = read(fd, &item, sizeof(ScaledData));
        if (bytesRead == -1) {
            perror("read scaledData failed");
            close(fd);
            return -1;
        }
        if (bytesRead == 0) break;

        if (strcmp(item.title, "__END__") == 0) {
            endCounter++;
            if (endCounter == numProcesses) break;
            continue;
        }

        dataList.push_back(item);
    }

    cout << "----------------CHECK-OUTPUT----------------" << endl;
    // printDataList(dataList);
    cout << "Data list size: " << dataList.size() << endl;

    close(fd);
    return 0;
}

int Output::makeOutputFile(const string &filePath)
{
    sortDataList();
    ofstream file(filePath);
    if (!file.is_open()) {
        perror("open failed (output)");
        return -1;
    }

    // Header
    file << "Title,Criterion" << endl;

    // Data
    for (const auto &item : dataList) {
        file << "\"" << item.title << "\"," << item.criterion << endl;
    }

    file.close();
    return 0;
}

void Output::sortDataList()
{
    std::sort(dataList.begin(), dataList.end(), [](const ScaledData &a, const ScaledData &b) {
        return a.criterion > b.criterion; // Sort in descending order
    });
}