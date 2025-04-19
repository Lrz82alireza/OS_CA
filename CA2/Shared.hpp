#ifndef SHARED_HPP
#define SHARED_HPP

#include <iostream>      // cout و cin
#include <unistd.h>      // fork(), pipe(), read(), write(), close(), _exit()
#include <sys/wait.h>    // waitpid()
#include <cstring>       // strcpy, strlen و توابع مشابه
#include <cstdlib>       // exit()
#include <sys/types.h>  // برای pid_t
#include <string>        
#include <vector>       
#include <fstream>
#include <sstream>

using namespace std;

#define READ_END 0
#define WRITE_END 1

#define FIELD_SIZE 512

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
    float discountedPrice[FIELD_SIZE];
    int recentReviewsSummary[FIELD_SIZE];
    int allReviewsSummary[FIELD_SIZE];
    int recentReviewsNumber[FIELD_SIZE];
    int allReviewsNumber[FIELD_SIZE];
};


#endif // SHARED_HPP