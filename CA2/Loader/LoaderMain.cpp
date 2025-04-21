#include "Loader.hpp"


int main()
{
    Loader loader(NAMED_PIPE_PATH);
    loader.receiveDataFromTransformer();
    unlink(NAMED_PIPE_PATH);
    return 0;
}