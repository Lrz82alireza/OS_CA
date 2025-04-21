#include "Extractor.hpp"

ExtractedData Extractor::extractLine(const string& line)
{
    ExtractedData data;

    string field;
    stringstream ss(line);

    int col = 0;
    while (getline(ss, field, ','))
    {
        switch (col) {
            case 0: safeCopy(data.title, field.c_str()); break;
            case 1: safeCopy(data.originalPrice, field.c_str()); break;
            case 2: safeCopy(data.discountedPrice, field.c_str()); break;
            case 3: safeCopy(data.link, field.c_str()); break;
            case 4: safeCopy(data.gameDescription, field.c_str()); break;
            case 5: safeCopy(data.recentReviewsSummary, field.c_str()); break;
            case 6: safeCopy(data.allReviewsSummary, field.c_str()); break;
            case 7: safeCopy(data.recentReviewsNumber, field.c_str()); break;
            case 8: safeCopy(data.allReviewsNumber, field.c_str()); break;
            case 9: safeCopy(data.developer, field.c_str()); break;
            case 10: safeCopy(data.publisher, field.c_str()); break;
            case 11: safeCopy(data.popularTags, field.c_str()); break;
            case 12: safeCopy(data.gameFeatures, field.c_str()); break;  
            case 13: safeCopy(data.minimumRequirements, field.c_str()); break;
            default: break;
        }
        col++;
    }

    return data;
}

vector<ExtractedData> Extractor::extract(string path)
{
    vector<ExtractedData> dataList;
    this->path = path;
    
    // Open the CSV file
    ifstream file(path);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << path << endl;
        return dataList;
    }
    string line;
    getline(file, line);

    while (getline(file, line))
    {
        dataList.push_back(this->extractLine(line));
    }

    return dataList;
}

int Extractor::sendDataToTransformer(const std::vector<ExtractedData>& dataList) {
    cout << path << " -> has size: " << dataList.size() << endl;

    for (const auto& item : dataList) {
        if (strcmp(item.title, "") == 0) {
            continue; // Skip empty items
        }
        if (write(fd[WRITE_END], &item, sizeof(ExtractedData)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    ExtractedData endMarker = {.title = "", 
                               .originalPrice = "",
                               .discountedPrice = "",
                               .link = "",
                               .gameDescription = "",
                               .recentReviewsSummary = "",
                               .allReviewsSummary = "",
                               .recentReviewsNumber = "",
                               .allReviewsNumber = "",
                               .developer = "",
                               .publisher = "",
                               .popularTags = "",
                               .gameFeatures = "",
                               .minimumRequirements = ""};
    strncpy(endMarker.title, "__END__", FIELD_SIZE - 1);
    // printData(endMarker);
    if (write(fd[WRITE_END], &endMarker, sizeof(ExtractedData)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(fd[WRITE_END]);
    return 1;
}
