#include "Loader.hpp"

void Loader::processMinMax(const TransformerData &item)
{
    // min
    minData.originalPrice = min(minData.originalPrice, item.originalPrice);
    minData.discountedPrice = min(minData.discountedPrice, item.discountedPrice);
    minData.recentReviewsSummary = min(minData.recentReviewsSummary, item.recentReviewsSummary);
    minData.allReviewsSummary = min(minData.allReviewsSummary, item.allReviewsSummary);
    minData.recentReviewsNumber = min(minData.recentReviewsNumber, item.recentReviewsNumber);
    minData.allReviewsNumber = min(minData.allReviewsNumber, item.allReviewsNumber);
    // max
    maxData.originalPrice = max(maxData.originalPrice, item.originalPrice);
    maxData.discountedPrice = max(maxData.discountedPrice, item.discountedPrice);
    maxData.recentReviewsSummary = max(maxData.recentReviewsSummary, item.recentReviewsSummary);
    maxData.allReviewsSummary = max(maxData.allReviewsSummary, item.allReviewsSummary);
    maxData.recentReviewsNumber = max(maxData.recentReviewsNumber, item.recentReviewsNumber);
    maxData.allReviewsNumber = max(maxData.allReviewsNumber, item.allReviewsNumber);
}

int Loader::receiveDataFromTransformer() {
    int flag = 0;
    int fd = open(NAMED_PIPE_PATH, O_RDONLY);
    int dummy_fd = open(NAMED_PIPE_PATH, O_WRONLY); // جلوگیری از EOF زودرس

    if (fd == -1 || dummy_fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    TransformerData item;
    int endCounter = 0;
    while (true) {
        ssize_t r = read(fd, &item, sizeof(TransformerData));
        if (r == -1) {
            if (errno == EINTR) continue;
            perror("read failed");
            break;
        }

        if (r == 0) break;

        if (strncmp(item.title, "__END__", FIELD_SIZE) == 0) {
            endCounter++;
            if (endCounter == PROC_NUM) break;
            continue;
        }

        if (flag == 0) {
            minData = item;
            maxData = item;
            flag = 1;
        } else {
            processMinMax(item);
        }

        dataList.push_back(item);
    }

    close(fd);
    close(dummy_fd);

    // Print min and max data
    cout << "----------------END----------------" << endl;
    safeCopy(minData.title, "Min Data");
    safeCopy(maxData.title, "Max Data");
    printData(minData);
    printData(maxData);

    return 0;
}
