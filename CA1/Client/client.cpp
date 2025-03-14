#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "SHARED.h"

#define UDP_PORT 8081

class Client {
private:
    int tcp_sock;
    int udp_sock;
    char username[50];
    char role[20];
    int port;
    const char* server_ip;

public:
    Client(const char* username, int port, const char* role, const char* server_ip = "127.0.0.1")
        : port(port), server_ip(server_ip), tcp_sock(-1) {
        strncpy(this->username, username, sizeof(this->username) - 1);
        strncpy(this->role, role, sizeof(this->role) - 1);
    }

    // تابع برای ایجاد و اتصال سوکت TCP به سرور
    int connectToTcpServer(const char* server_ip, int port) {
        int sock = create_socket(false, false);
        connect_socket(sock, server_ip, port);
        return sock;
    }

    // تابع برای دریافت پورت جدید از سرور
    int receiveNewPort(int sock) {
        char buffer[10];  // برای ذخیره پورت جدید
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) {
            my_print("Failed to receive new port.\n");
            close(sock);
            return -1;  // خطا در دریافت پورت جدید
        }
        buffer[len] = '\0';  // اضافه کردن null terminator
        return atoi(buffer);  // تبدیل به عدد صحیح
    }

    // تابع برای بستن اتصال TCP اولیه
    void closeTcpConnection(int sock) {
        close(sock);
    }

    // تابع برای ارسال اطلاعات کاربر به سرور
    void sendUserInfo(int sock, const char* username, const char* role) {
        Client_info client_info;
        strncpy(client_info.username, username, sizeof(client_info.username) - 1);
        strncpy(client_info.role, role, sizeof(client_info.role) - 1);
        send(sock, &client_info, sizeof(client_info), 0);
    }

    // تابع برای ایجاد و تنظیم سوکت UDP
    int setupUdpSocket(int udp_port) {
        int udp_sock = create_socket(true, false);

        // reuse address and port
        int opt = 1;
        setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        #ifdef SO_REUSEPORT
        setsockopt(udp_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        #endif

        bind_socket(udp_sock, udp_port, false);
        return udp_sock;
    }

    // تابع اصلی برای اتصال به سرور
    void connectToServer() {
        // ایجاد و اتصال سوکت TCP به سرور
        tcp_sock = connectToTcpServer(server_ip, port);

        // دریافت پورت جدید از سرور
        int new_port = receiveNewPort(tcp_sock);
        if (new_port < 0) return;  // در صورت خطا در دریافت پورت، تابع متوقف می‌شود

        my_print("New assigned port: ");
        my_print(std::to_string(new_port).c_str());
        my_print("\n");

        // بستن اتصال اولیه
        closeTcpConnection(tcp_sock);

        // اتصال به پورت جدید
        tcp_sock = connectToTcpServer(server_ip, new_port);
        my_print("Client ");
        my_print(username);
        my_print(" connected to server on port ");
        my_print(std::to_string(new_port).c_str());
        my_print(" on fd ");
        my_print(std::to_string(tcp_sock).c_str());
        my_print("\n");

        
        // ارسال اطلاعات کاربر به سرور
        sendUserInfo(tcp_sock, username, role);
        
        // ایجاد و تنظیم سوکت UDP
        my_print("Debugging: \n");
        udp_sock = setupUdpSocket(UDP_PORT);
        my_print("Debugging: \n");

        my_print("Connected to server as ");
        my_print(username);
        my_print(" (");
        my_print(role);
        my_print(") on new port ");
        my_print(std::to_string(new_port).c_str());
        my_print("\n");

        startClient();
    }

    // تابع برای آماده‌سازی مجموعه
    void prepareFdSet(fd_set& read_fds, int tcp_sock, int udp_sock) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(tcp_sock, &read_fds);
        FD_SET(udp_sock, &read_fds);
    }    

    void handleUserInput(int tcp_sock) {
        char buffer[1024];
        int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';
            send(tcp_sock, buffer, len, 0);
        }
    }

    bool handleTcpMessage(int tcp_sock) {
        char buffer[1024];
        int len = recv(tcp_sock, buffer, sizeof(buffer) - 1, 0);
        if (len > 0) {
            buffer[len] = '\0';
            my_print("[TCP] Server: ");
            my_print(buffer);
            my_print("\n");
            return true;
        } else {
            my_print("Server disconnected.\n");
            return false;
        }
    }
    
    void handleUdpMessage(int udp_sock) {
        char buffer[1024];
        struct sockaddr_in server_addr;
        socklen_t addr_len = sizeof(server_addr);
        int len = recvfrom(udp_sock, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr*)&server_addr, &addr_len);
        if (len > 0) {
            buffer[len] = '\0';
            my_print("[UDP] Broadcast: ");
            my_print(buffer);
            my_print("\n");
        }
    }
    
    void startClient() {
        fd_set read_fds;
    
        while (true) {
            prepareFdSet(read_fds, tcp_sock, udp_sock);
            int max_fd = std::max(tcp_sock, udp_sock) + 1;
            int activity = select(max_fd, &read_fds, NULL, NULL, NULL);
    
            if (activity < 0) {
                perror("select() failed");
                break;
            }
    
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                handleUserInput(tcp_sock);
            }
    
            if (FD_ISSET(tcp_sock, &read_fds)) {
                if (!handleTcpMessage(tcp_sock)) {
                    break;
                }
            }
    
            if (FD_ISSET(udp_sock, &read_fds)) {
                handleUdpMessage(udp_sock);
            }
        }
    
        close(tcp_sock);
        close(udp_sock);
    }
    


    ~Client() {
        if (tcp_sock != -1) close(tcp_sock);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        my_print("Usage: ");
        my_print(argv[0]);
        my_print(" <username> <port> <role>\n");
        return EXIT_FAILURE;
    }

    const char* username = argv[1];
    int port = atoi(argv[2]);
    const char* role = argv[3];

    Client client(username, port, role);
    client.connectToServer();

    return 0;
}
