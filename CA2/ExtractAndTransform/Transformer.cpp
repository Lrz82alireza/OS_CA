#include "Transformer.hpp"

int Transformer::run()
{
    transformedDataList = this->transform(this->readDataFromExtractor());
    this->sendDataToLoader(this->transformedDataList);
    return 0;
}

int Transformer::sendDataToLoader(vector<TransformerData> dataList)
{
    int fd = open(NAMED_PIPE_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    int d_size = dataList.size();
    if (write(fd, &d_size, sizeof(int)) == -1)
    {
        perror("write size failed");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &dataList[0], d_size * sizeof(TransformerData)) == -1)
    {
        perror("write data failed");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return 0;
}

vector<TransformerData> Transformer::transform(const vector<ExtractedData>& dataList)
{
    printf("Transforming...\n");

    vector<TransformerData> transformedDataList;
    // transformedDataList.reserve(dataList.size());

    for (const auto& data : dataList)
    {
        transformedDataList.push_back(this->transformLine(data));
    }

    return transformedDataList;
}

TransformerData Transformer::transformLine(const ExtractedData& data)
{
    TransformerData transformedData;
    
    // title
    strcpy(transformedData.title, data.title);

    // originalPrice
    transformedData.originalPrice = atof(substr(data.originalPrice, 1, strlen(data.originalPrice) - 1).c_str());
    // discountedPrice
    transformedData.discountedPrice = priceToPercent(transformedData.originalPrice
                                                        , atof(substr(data.discountedPrice, 1, strlen(data.discountedPrice) - 1).c_str()));

    // recentReviewsSummary
    transformedData.recentReviewsSummary = ReviewsSummaryMap[data.recentReviewsSummary];
    // allReviewsSummary
    transformedData.allReviewsSummary = ReviewsSummaryMap[data.allReviewsSummary];
    
    // recentReviewsNumber
    transformedData.recentReviewsNumber = extractReviewsNumber(data.recentReviewsNumber);

    // allReviewsNumber
    transformedData.allReviewsNumber = extractReviewsNumber(data.allReviewsNumber);

    return transformedData;
}

int Transformer::extractReviewsNumber(const string& str)
{
    stringstream ss(str);
    string token;
    int number = 0;

    while (getline(ss, token, ' '))
    {
        if (!token.empty() && all_of(token.begin(), token.end(), ::isdigit))
        {
            number = stoi(token);
            break;
        }
    }

    return number;
}

float Transformer::priceToPercent(float price, float discount)
{
    if (discount == 0)
        return 1;
    return (price - discount) / price * 100;
}

std::vector<ExtractedData> Transformer::readDataFromExtractor() {
    close(fd[WRITE_END]);

    std::vector<ExtractedData> dataList;
    ExtractedData item;

    while (true) {
        ssize_t r = read(fd[READ_END], &item, sizeof(ExtractedData));
        if (r == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (r == 0) break;

        if (strncmp(item.title, "__END__", FIELD_SIZE) == 0) break;

        dataList.push_back(item);
    }

    close(fd[READ_END]);
    return dataList;
}
