#include "Transformer.hpp"

int Transformer::run()
{
    transformedDataList = this->transform(this->readDataFromExtractor());
    this->sendDataToLoader(this->transformedDataList);
    return 0;
}

int Transformer::sendDataToLoader(vector<TransformerData> dataList)
{
    
}

vector<TransformerData> Transformer::transform(const vector<ExtractedData>& dataList)
{
    vector<TransformerData> transformedDataList;
    transformedDataList.reserve(dataList.size());

    for (const auto& data : dataList)
    {
        transformedDataList.push_back(this->transformLine(data));
    }

    return transformedDataList;
}

TransformerData Transformer::transformLine(const ExtractedData& data)
{
    TransformerData transformedData;
    string temp;
    
    // title
    strcpy(transformedData.title, data.title);

    // originalPrice
    temp = data.originalPrice;
    transformedData.originalPrice = atof(temp.substr(1).c_str());
    // discountedPrice
    temp = data.discountedPrice;
    transformedData.discountedPrice = priceToPercent(transformedData.originalPrice
                                                        , atof(temp.substr(1).c_str()));

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

vector<ExtractedData> Transformer::readDataFromExtractor()
{
    close(fd[WRITE_END]);
    vector<ExtractedData> dataList;
    int d_size;
    if (read(fd[READ_END], &d_size, sizeof(int)) == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    dataList.resize(d_size);

    if (read(fd[READ_END], &dataList, d_size * sizeof(ExtractedData)) == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(fd[READ_END]);
    return dataList;
}