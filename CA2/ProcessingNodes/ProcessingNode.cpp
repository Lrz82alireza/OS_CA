#include "ProcessingNode.hpp"

int ProcessingNode::run()
{
    cout << "[Node] PID " << pid << " ready for processing..." << endl;

    // فعلاً فقط منتظر می‌مونه که بعداً Loader داده‌ها رو بفرسته
    pause(); // این باعث میشه پردازه بخوابه تا زمانی که سیگنالی دریافت کنه

    return 0;
}
