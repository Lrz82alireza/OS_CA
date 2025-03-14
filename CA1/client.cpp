#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "myprint.h"

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

    void connectToServer() {
        struct sockaddr_in server_addr;

        // ایجاد سوکت tcp
        tcp_sock = create_socket(false, false);
        connect_socket(tcp_sock, server_ip, port);

        // 🔹 دریافت پورت جدید از سرور
        char buffer[10];  // برای ذخیره پورت جدید
        int len = recv(tcp_sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) {
            my_print("Failed to receive new port.\n");
            close(tcp_sock);
            return;
        }
        buffer[len] = '\0';  // اضافه کردن null terminator
        int new_port = atoi(buffer);  // تبدیل به عدد صحیح

        my_print("New assigned port: ");
        my_print(buffer);
        my_print("\n");

        // 🔹 بستن اتصال اولیه
        close(tcp_sock);
        
        // 🔹 اتصال به پورت جدید
        tcp_sock = create_socket(false, false);
        connect_socket(tcp_sock, server_ip, new_port);

        // 🔹 ارسال اطلاعات کاربر به سرور
        Client_info client_info;
        strncpy(client_info.username, username, sizeof(client_info.username) - 1);
        strncpy(client_info.role, role, sizeof(client_info.role) - 1);
        send(tcp_sock, &client_info, sizeof(client_info), 0);


        // ایجاد سوکت udp
        udp_sock = create_socket(true, false);
        bind_socket(udp_sock, UDP_PORT, false);

        my_print("Connected to server as ");
        my_print(username);
        my_print(" (");
        my_print(role);
        my_print(") on new port ");
        my_print(std::to_string(new_port).c_str());
        my_print("\n");

        startClient();
        
    }

    void startClient() {
        fd_set read_fds;
        char buffer[1024];
    
        while (true) {
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO, &read_fds);  // ورودی استاندارد (کیبورد)
            FD_SET(tcp_sock, &read_fds);      // سوکت TCP
            FD_SET(udp_sock, &read_fds);      // سوکت UDP
    
            int max_fd = std::max(tcp_sock, udp_sock) + 1;
            int activity = select(max_fd, &read_fds, NULL, NULL, NULL);
    
            if (activity < 0) {
                perror("select() failed");
                break;
            }
    
            // 🔹 خواندن از کیبورد و ارسال به سرور از طریق TCP
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                if (len > 0) {
                    buffer[len] = '\0';
                    send(tcp_sock, buffer, len, 0);
                }
            }
    
            // 🔹 دریافت پیام از سرور (TCP)
            if (FD_ISSET(tcp_sock, &read_fds)) {
                int len = recv(tcp_sock, buffer, sizeof(buffer) - 1, 0);
                if (len > 0) {
                    buffer[len] = '\0';
                    my_print("[TCP] Server: ");
                    my_print(buffer);
                    my_print("\n");
                } else {
                    my_print("Server disconnected.\n");
                    break;
                }
            }
    
            // 🔹 دریافت پیام از UDP
            if (FD_ISSET(udp_sock, &read_fds)) {
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
