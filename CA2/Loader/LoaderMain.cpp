#include "Loader.hpp"


int main()
{
    // Create the named pipe
    if (mkfifo(NAMED_PIPE_PATH, 0666) == -1)
    {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }

    Loader loader(NAMED_PIPE_PATH);
    loader.receiveDataFromTransformer();
    unlink(NAMED_PIPE_PATH);
    return 0;
}