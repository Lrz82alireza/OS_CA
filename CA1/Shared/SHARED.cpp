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

// تابع to_string برای تبدیل int به رشته
std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// تابع to_string برای تبدیل double به رشته
std::string to_string(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// تابع to_string برای تبدیل const char* به رشته
std::string to_string(const char* value) {
    return std::string(value);
}

// تابع برای ایجاد سوکت
int create_socket(bool is_udp, bool is_broadcast) {
    int sock_fd = socket(AF_INET, (is_udp ? SOCK_DGRAM : SOCK_STREAM), 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
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
        perror("Invalid address");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        my_print("Debbugging \n");
        perror("Connect failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
}