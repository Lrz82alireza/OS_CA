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
            case 0: strcpy(data.title, field.c_str()); break;
            case 1: strcpy(data.originalPrice, field.c_str()); break;
            case 2: strcpy(data.discountedPrice, field.c_str()); break;
            case 3: strcpy(data.link, field.c_str()); break;
            case 4: strcpy(data.gameDescription, field.c_str()); break;
            case 5: strcpy(data.recentReviewsSummary, field.c_str()); break;
            case 6: strcpy(data.allReviewsSummary, field.c_str()); break;
            case 7: strcpy(data.recentReviewsNumber, field.c_str()); break;
            case 8: strcpy(data.allReviewsNumber, field.c_str()); break;
            case 9: strcpy(data.developer, field.c_str()); break;
            case 10: strcpy(data.publisher, field.c_str()); break;
            case 11: strcpy(data.popularTags, field.c_str()); break;
            case 12: strcpy(data.gameFeatures, field.c_str()); break;  
            case 13: strcpy(data.minimumRequirements, field.c_str()); break;
            default: break;
        }
        col++;
    }

    return data;
}

vector<ExtractedData> Extractor::extract(string path)
{
    vector<ExtractedData> dataList;
    
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

int Extractor::sendDataToTransformer(vector<ExtractedData> dataList){
    close(fd[READ_END]);
    if (write(fd[WRITE_END], dataList.data(), dataList.size() * sizeof(ExtractedData)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd[WRITE_END]);
    return 1;
}