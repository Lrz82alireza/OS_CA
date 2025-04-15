#include "Extractor.hpp"
#include "Transformer.hpp"

#include "Def.hpp"

#define PROC_NUM 3

int main ()
{
    for (int i = 0; i < PROC_NUM; i++)
    {
        int fd[2];
        // Create a unnamed pipe
        if (pipe(fd) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
    }
}