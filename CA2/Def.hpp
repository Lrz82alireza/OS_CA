#include <iostream>      // برای cout و cin
#include <unistd.h>      // برای fork(), pipe(), read(), write(), close(), _exit()
#include <sys/wait.h>    // برای waitpid()
#include <cstring>       // برای strcpy, strlen و توابع مشابه
#include <cstdlib>       // برای exit()
#include <string>        // برای استفاده از std::string
#include <vector>       // برای استفاده از std::vector
#include <sys/types.h>  // برای pid_t

using namespace std;