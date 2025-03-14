#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "myprint.h"

#define UDP_PORT 8081

class Server {
private:
    int server_fd;
    int stp_port;
    
    int udp_socket;
    int udp_port = UDP_PORT;
    
public:
    Server(int port) : stp_port(port), server_fd(-1) {}

    void start() {
        // ایجاد سوکت TCP (سرور)
        server_fd = create_socket(false, false);
        bind_socket(server_fd, stp_port, false);
        if (listen(server_fd, 5) < 0) {
            my_print("listen failed\n");
            exit(EXIT_FAILURE);
        }
    
        // ایجاد سوکت UDP فقط برای ارسال Broadcast
        udp_socket = create_socket(true, true);
    
        my_print("Server listening on port ");
        my_print(std::to_string(stp_port).c_str());
        my_print("\n");
    
        // راه‌اندازی پردازش TCP و UDP
        startServer();
    }
    
    void startServer() {
        fd_set read_fds;
        char buffer[1024];
    
        struct sockaddr_in broadcast_addr;
        memset(&broadcast_addr, 0, sizeof(broadcast_addr));
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_port = htons(udp_port);
        broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;  // ارسال به تمام کلاینت‌ها
    
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
    
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            my_print("accept failed\n");
            return;
        }
        my_print("New client connected\n");
    
        while (true) {
            FD_ZERO(&read_fds);
            FD_SET(client_fd, &read_fds);  // پیام‌های TCP از کلاینت
            FD_SET(udp_socket, &read_fds);  // ارسال پیام Broadcast
    
            int max_fd = std::max(client_fd, udp_socket) + 1;
            int activity = select(max_fd, &read_fds, NULL, NULL, NULL);
    
            if (activity < 0) {
                perror("select() failed");
                break;
            }

            // دریافت پیام از کلاینت (TCP)
            if (FD_ISSET(client_fd, &read_fds)) {
                int len = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                if (len > 0) {
                    buffer[len] = '\0';
                    my_print("[TCP] Client: ");
                    my_print(buffer);
                    my_print("\n");
    
                    // ارسال پاسخ به کلاینت
                    send(client_fd, "Received your message", 21, 0);
                    
                    strcpy(buffer, "Hello to all clients!");
                    sendto(udp_socket, buffer, strlen(buffer), 0,
                           (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
                } else {
                    my_print("Client disconnected.\n");
                    break;
                }
            }
    
            // ارسال پیام Broadcast از طریق UDP
            if (FD_ISSET(udp_socket, &read_fds)) {
                strcpy(buffer, "Hello to all clients!");
                sendto(udp_socket, buffer, strlen(buffer), 0,
                       (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
            }
        }
    
        close(client_fd);
        close(udp_socket);
    }
    
    


    ~Server() {
        if (server_fd != -1) close(server_fd);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        my_print("Usage: ");
        my_print(argv[0]);
        my_print(" <port>\n");
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    Server server(port);
    server.start();

    return 0;
}
