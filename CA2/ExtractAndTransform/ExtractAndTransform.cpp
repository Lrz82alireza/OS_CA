#include "Extractor.hpp"
#include "Transformer.hpp"

#include "Shared.hpp"

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
        close(fd[READ_END]);
        Extractor extr(pid, fd);

        string path = PATH + to_string(index + 1) + ".csv";

        extr.sendDataToTransformer(extr.extract(path));
        _exit(0);
    }
    return 0;
}

int calltransformer(pid_t pid, int fd[2]){
    if (pid == 0)
    {
        close(fd[WRITE_END]);
        Transformer trans(pid, fd);
        trans.run();
        _exit(0);
    }
    return 0;
}

int callLoader(){
    if (fork() == 0)
    {
        execl("./bin/Loader", "Loader", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int main ()
{
    // Create a named pipe for LOADER
    mkfifo(NAMED_PIPE_PATH, 0666);

    int fd[2];
    for (int i = 0; i < PROC_NUM; i++)
    {
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

    for (int i = 0; i < 2 * PROC_NUM; i++)
    {
        close(fd[READ_END]);
        close(fd[WRITE_END]);
    }

    for (int i = 0; i < 2 * PROC_NUM; i++) {
        wait(NULL);
    }
    

}