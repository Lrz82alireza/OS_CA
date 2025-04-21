#include "ProcessingNode.hpp"

int ProcessingNode::run()
{

    // فعلاً فقط منتظر می‌مونه که بعداً Loader داده‌ها رو بفرسته
    pause(); // این باعث میشه پردازه بخوابه تا زمانی که سیگنالی دریافت کنه

    return 0;
}

void ProcessingNode::listenAndCompute()
{
    int fd = open(pipePath.c_str(), O_RDONLY);
    if (fd == -1)
    {
        perror("open named pipe failed");
        exit(EXIT_FAILURE);
    }

    TransformerData item;
    while (true)
    {
        ssize_t r = read(fd, &item, sizeof(TransformerData));
        if (r == -1)
        {
            perror("read failed");
            break;
        }
        else if (r == 0)
        {
            break; // EOF
        }

        if (strncmp(item.title, "__END__", FIELD_SIZE) == 0)
        {
            break; // End marker
        }

        printData(item);
    }

    close(fd);
}