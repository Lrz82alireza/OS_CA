#include "Loader.hpp"


int main()
{
    // get Data from transformer
    Loader loader(NAMED_PIPE_PATH);
    loader.receiveDataFromTransformer();
    unlink(NAMED_PIPE_PATH);

    // get ProcInfos from Processing
    mkdir("../pipes", 0777);

    unlink(LOADER_PROCESSOR_PIPE_PATH);
    if (mkfifo(LOADER_PROCESSOR_PIPE_PATH, 0666) == -1) {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }

    // call ProcessingMain
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        execl("./bin/processor", "processor", to_string(PROC_NUM).c_str(), LOADER_PROCESSOR_PIPE_PATH, NULL);
        perror("execl for ProcessingMain failed");
        exit(EXIT_FAILURE);
    }

    // read ProcInfos from Processing
    loader.receiveProcInfoFromProcessing(LOADER_PROCESSOR_PIPE_PATH);
    unlink(LOADER_PROCESSOR_PIPE_PATH);

    // chunk data

    // send data to ProcessingNodes

    wait(NULL);

    return 0;
}