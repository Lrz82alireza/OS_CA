#include "Transformer.hpp"

int Transformer::run()
{
    transformedDataList = this->transform(this->readDataFromExtractor());
    // printDataList(this->transformedDataList);
    this->sendDataToLoader(this->transformedDataList);
    return 0;
}

int Transformer::sendDataToLoader(const std::vector<TransformerData>& dataList)
{
    printf("Sending data to loader...\n");
    close(fd[READ_END]);

    int fd = open(NAMED_PIPE_PATH, O_WRONLY); 
    if (fd == -1)
    {
        perror("open named pipe failed");
        exit(EXIT_FAILURE);
    }

    for (const auto& item : dataList)
    {
        if (write(fd, &item, sizeof(TransformerData)) == -1)
        {
            perror("write data failed");
            exit(EXIT_FAILURE);
        }
    }

    TransformerData endMarker = {};
    strncpy(endMarker.title, "__END__", FIELD_SIZE - 1);
    endMarker.title[FIELD_SIZE - 1] = '\0';

    if (write(fd, &endMarker, sizeof(TransformerData)) == -1)
    {
        perror("write end marker failed");
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
        try
        {
            transformedDataList.push_back(this->transformLine(data));
        }
        catch(const std::exception& e)
        {
            // printData(data);
            // std::cout << "Here in " << e.what() << '\n';
        }
        
    }

    return transformedDataList;
}

TransformerData Transformer::transformLine(const ExtractedData& data)
{
    TransformerData transformedData;
    
    // title
    safeCopy(transformedData.title, data.title);

    // originalPrice
    // printData(data);
    transformedData.originalPrice = atof(substr_(data.originalPrice, 1, strlen(data.originalPrice) - 1).c_str());
    
    // discountedPrice
    transformedData.discountedPrice = priceToPercent(transformedData.originalPrice
                                                        , atof(substr_(data.discountedPrice, 1, strlen(data.discountedPrice) - 1).c_str()));

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
    float result = (price - discount) / price * 100;
    if (result == 0)
    {
        return 1;
    }
    return result;
}

ssize_t readFull(int fd, void* buffer, size_t size) {
    size_t total = 0;
    char* buf = reinterpret_cast<char*>(buffer);
    while (total < size) {
        ssize_t r = read(fd, buf + total, size - total);
        if (r == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (r == 0) {
            break; // EOF
        }
        total += r;
    }
    return total;
}

std::vector<ExtractedData> Transformer::readDataFromExtractor() {
    std::vector<ExtractedData> dataList;
    ExtractedData item;

    while (true) {
        ssize_t r = readFull(fd[READ_END], &item, sizeof(ExtractedData));
        if (r == 0) {
            break; // EOF
        } else if (r != sizeof(ExtractedData)) {
            std::cerr << "Partial read! Expected " << sizeof(ExtractedData)
                      << ", got " << r << std::endl;
            exit(EXIT_FAILURE);
        }

        dataList.push_back(item);
    }

    close(fd[READ_END]);
    cout << "Received size: " << dataList.size() << endl;
    return dataList;
}
