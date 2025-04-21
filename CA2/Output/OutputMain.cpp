#include "Output.hpp"

int main() {
    // output Pipe
    unlink(OUTPUT_PROCESSOR_PIPE_PATH);
    if (mkfifo(OUTPUT_PROCESSOR_PIPE_PATH, 0666) == -1) {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }

    cout << "Receiving scaled data from processing nodes in Output...." << endl;
    // call OutputMain
    Output output(OUTPUT_PROCESSOR_PIPE_PATH, PROC_NUM);
    output.receiveDataFromProcessingNodes();

    

    unlink(OUTPUT_PROCESSOR_PIPE_PATH);

    return 0;
}