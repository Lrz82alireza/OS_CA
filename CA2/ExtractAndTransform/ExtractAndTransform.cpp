#include "Extractor.hpp"
#include "Transformer.hpp"

#include "Shared.hpp"

#define PROC_NUM 3

const string PATH = "./assets/steamdb";

int checkForkError(pid_t pid) {
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int callExtractor(pid_t pid, int fd[2], int index) {
    if (pid == 0)
    {
        Extractor extr(pid, fd);

        string path = PATH + to_string(index + 1) + ".csv";

        extr.sendDataToTransformer(extr.extract(path));
    }
    return 0;
}

int calltransformer(pid_t pid, int fd[2]){
    if (pid == 0)
    {
        Transformer trans(pid, fd);
        trans.run();
    }
    return 0;
}

int main ()
{
    // Create a named pipe for LOADER
    mkfifo(NAMED_PIPE_PATH, 0666);

    for (int i = 0; i < PROC_NUM; i++)
    {
        int fd[2];
        // Create a unnamed pipe
        if (pipe(fd) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // Create child for Extractor
        pid_t pid = fork();
        checkForkError(pid);
        callExtractor(pid, fd, i);

        // Create child for Transformer
        pid_t pid2 = fork();
        checkForkError(pid2);
        calltransformer(pid2, fd);

    }
}