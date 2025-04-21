#include "ProcessingNode.hpp"

void ProcessingNode::listenAndCompute()
{
    receiveDataFromLoader();
    computeScaledDataList();
    cout << "Node " << id << " has " << dataList.size() << " data items." << endl;
}

int ProcessingNode::receiveMinMaxFromLoader()
{
    int fd = open(pipePath.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("open failed (min/max)");
        return -1;
    }

    if (read(fd, &minData, sizeof(TransformerData)) == -1) {
        perror("read minData failed");
        close(fd);
        return -1;
    }

    if (read(fd, &maxData, sizeof(TransformerData)) == -1) {
        perror("read maxData failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int ProcessingNode::computeScaledDataList()
{
    for (const auto &item : dataList) {
        ScaledData scaledData = computeScaledData(item);
        scaledDataList.push_back(scaledData);
    }
    return 0;
}

float ProcessingNode::computeScaled(float value, float min, float max)
{
    if (max - min != 0) {
        return ((value - min) / (max - min)) + 1.0f;
    }
    return 0.0f;
}

ScaledData ProcessingNode::computeScaledData(const TransformerData &data)
{
    ScaledData scaledData;
    TransformerData tempData;
    strcpy(scaledData.title, data.title);
    tempData.originalPrice = computeScaled(data.originalPrice, minData.originalPrice, maxData.originalPrice);
    tempData.discountedPrice = computeScaled(data.discountedPrice, minData.discountedPrice, maxData.discountedPrice);
    tempData.recentReviewsSummary = computeScaled(data.recentReviewsSummary, minData.recentReviewsSummary, maxData.recentReviewsSummary);
    tempData.allReviewsSummary = computeScaled(data.allReviewsSummary, minData.allReviewsSummary, maxData.allReviewsSummary);
    tempData.recentReviewsNumber = computeScaled(data.recentReviewsNumber, minData.recentReviewsNumber, maxData.recentReviewsNumber);
    tempData.allReviewsNumber = computeScaled(data.allReviewsNumber, minData.allReviewsNumber, maxData.allReviewsNumber);
    scaledData.criterion = computeCriterion(tempData);
    return scaledData;
}

float ProcessingNode::computeCriterion(const TransformerData &scaledData)
{
    float criterion = 0.0f;
    float top = 10.0f * scaledData.discountedPrice *
                    scaledData.recentReviewsSummary *
                    scaledData.allReviewsSummary *
                    scaledData.recentReviewsNumber *
                    scaledData.allReviewsNumber;
    float bottom = scaledData.originalPrice;
    criterion = top / bottom;
    return criterion;
}

int ProcessingNode::receiveDataFromLoader()
{
    cout << "NODE OPENED PATH: " << pipePath << endl;
    int fd = open(pipePath.c_str(), O_RDONLY);
    if (fd == -1) {
        perror(("open failed (data) for " + pipePath).c_str());
        return -1;
    }

    if (read(fd, &minData, sizeof(TransformerData)) == -1) {
        perror("read minData failed");
        close(fd);
        return -1;
    }

    if (read(fd, &maxData, sizeof(TransformerData)) == -1) {
        perror("read maxData failed");
        close(fd);
        return -1;
    }

    TransformerData item;
    while (true) {
        ssize_t bytesRead = read(fd, &item, sizeof(TransformerData));
        if (bytesRead == -1) {
            perror("read data failed");
            close(fd);
            return -1;
        }

        if (strcmp(item.title, "__END__") == 0) {
            break;
        }

        dataList.push_back(item);
    }

    close(fd);
    return 0;
}

