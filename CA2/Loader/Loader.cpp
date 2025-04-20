#include "Loader.hpp"

int Loader::receiveDataFromTransformer()
{
    int fd = open(NAMED_PIPE_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    TransformerData item;
    while (true) {
        ssize_t r = read(fd, &item, sizeof(TransformerData));
        if (r == -1) {
            if (errno == EINTR) continue;
            perror("read failed");
            break;
        }

        if (r == 0) break; // EOF - no more writers

        if (strncmp(item.title, "__END__", FIELD_SIZE) == 0)
            continue;

        // process min and max
        

        dataList.push_back(item);
    }

    close(fd);
    return 0;
}