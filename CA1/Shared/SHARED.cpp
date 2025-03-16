// SHARED.cpp
#include "SHARED.h"
#include <unistd.h>
#include <sstream>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


// پیاده‌سازی تابع print
void my_print(const char* str) {
    write(STDOUT_FILENO, str, strlen(str));
}

// تبدیل عدد صحیح به رشته
std::string to_string(int value) {
    char buffer[12]; // فضای کافی برای نمایش عدد صحیح (حداکثر 11 رقم + null)
    sprintf(buffer, "%d", value);
    return std::string(buffer);
}

// تبدیل عدد اعشاری به رشته
std::string to_string(double value) {
    char buffer[32]; // فضای کافی برای نمایش عدد اعشاری
    sprintf(buffer, "%.6f", value); // نمایش تا 6 رقم اعشار
    return std::string(buffer);
}

// تبدیل const char* به رشته
std::string to_string(const char* value) {
    if (value == nullptr) {
        return std::string(""); // جلوگیری از کرش در صورت nullptr
    }
    return std::string(value);
}

// تابع برای ایجاد سوکت
int create_socket(bool is_udp, bool is_broadcast) {
    int sock_fd = socket(AF_INET, (is_udp ? SOCK_DGRAM : SOCK_STREAM), 0);
    if (sock_fd < 0) {
        my_print("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // فعال‌سازی SO_REUSEADDR برای امکان استفاده مجدد از پورت
    int reuseAddr = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0) {
        perror("Failed to set SO_REUSEADDR");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // فعال‌سازی Broadcast در صورت نیاز
    if (is_udp && is_broadcast) {
        int broadcastEnable = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            perror("Failed to enable broadcast");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    return sock_fd;
}


// تابع برای Bind کردن سوکت به یک پورت
void bind_socket(int sock_fd, int port, bool is_broadcast) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = is_broadcast ? INADDR_BROADCAST : INADDR_ANY;

    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
}

// تابع برای اتصال کلاینت به سرور (TCP)
void connect_socket(int sock_fd, const char* server_ip, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        my_print("Invalid address");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        my_print("Connecting failed \n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
}

std::string extractType(const std::string& input) {
    if (input.empty() || input[0] != '/') return "";

    size_t spacePos = input.find(' ');
    if (spacePos == std::string::npos) {
        return input.substr(1);  // اگر فاصله‌ای نبود، کل رشته بعد از `/` رو برگردون
    }
    
    return input.substr(1, spacePos - 1);  // نوع پیام (type) رو استخراج کن
}

void sendMsgToTeam(Team* team, const std::string& msg) {
    if (team->coder != nullptr) {
        send(team->coder->client_fd, msg.c_str(), msg.length(), 0);
    }
    if (team->navigator != nullptr) {
        send(team->navigator->client_fd, msg.c_str(), msg.length(), 0);
    }
}