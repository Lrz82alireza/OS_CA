#include "Extractor.hpp"
#include "Transformer.hpp"
#include "Shared.hpp"

const string PATH = "./assets/steamdb";

int checkForkError(pid_t pid)
{
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int callExtractor(pid_t pid, int fd[2], int index)
{
    if (pid == 0)
    {
        close(fd[READ_END]);
        Extractor extr(getpid(), fd);
        string path = PATH + to_string(index + 1) + ".csv";
        extr.sendDataToTransformer(extr.extract(path));
        _exit(0);
    }
    return 0;
}

int callTransformer(pid_t pid, int fd[2])
{
    if (pid == 0)
    {
        close(fd[WRITE_END]);
        Transformer trans(getpid(), fd);
        trans.run();
        _exit(0);
    }
    return 0;
}

int callLoader()
{
    if (fork() == 0)
    {
        execl("./bin/loader", "loader", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int main()
{
    mkdir("../NP", 0777);

    unlink(NAMED_PIPE_PATH);
    if (mkfifo(NAMED_PIPE_PATH, 0666) == -1) {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }
    
    callLoader();
    sleep(1);
    
    
    int pipes[PROC_NUM][2];
    pid_t child_pids[2 * PROC_NUM];
    
    for (int i = 0; i < PROC_NUM; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        pid_t pid1 = fork();
        checkForkError(pid1);
        if (pid1 == 0) callExtractor(pid1, pipes[i], i);
        child_pids[2 * i] = pid1;
        
        pid_t pid2 = fork();
        checkForkError(pid2);
        if (pid2 == 0) callTransformer(pid2, pipes[i]);
        child_pids[2 * i + 1] = pid2;
        
        close(pipes[i][READ_END]);
        close(pipes[i][WRITE_END]);
    }
    
    for (int i = 0; i < 2 * PROC_NUM; i++)
    {
        waitpid(child_pids[i], NULL, 0);
    }
    
    wait(NULL);
    
    unlink(NAMED_PIPE_PATH);
    
    return 0;
}
