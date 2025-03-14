#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

#include "SHARED.h"

#define UDP_PORT 8081


class Server {
private:
    int server_fd;
    int stp_port;
    
    int udp_socket;
    int udp_port = UDP_PORT;

    std::vector<Client_info> clients;
    
    // تابع برای دریافت پورت اختصاص داده‌شده به کلاینت
    int getAssignedPort(int client_fd) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        getsockname(client_fd, (struct sockaddr*)&addr, &addr_len);
        return ntohs(addr.sin_port);
    }

    // تابع برای پذیرش اتصال جدید از کلاینت
    int acceptNewClient(int server_fd) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept() failed");
            return -1;
        }
        return client_fd;
    }

    // تابع برای اختصاص پورت جدید به کلاینت
    int assignNewPort() {
        return 5000 + clients.size();  // اختصاص پورت جدید از 5000 به بعد
    }

    // تابع برای ارسال پورت جدید به کلاینت
    void sendNewPortToClient(int client_fd, int new_port) {
        char port_msg[10];
        sprintf(port_msg, "%d", new_port);
        send(client_fd, port_msg, strlen(port_msg), 0);
        my_print("Assigned new port ->: ");
        my_print(port_msg);
        my_print("\n");
    }

    // تابع برای بستن اتصال اولیه با کلاینت
    void closeClientConnection(int client_fd) {
        close(client_fd);
    }

    // تابع برای انتظار و پذیرش اتصال مجدد کلاینت به پورت جدید
    int waitForClientOnNewPort(int new_server_fd) {
        struct sockaddr_in new_client_addr;
        socklen_t new_client_len = sizeof(new_client_addr);
        int new_client_fd = accept(new_server_fd, (struct sockaddr*)&new_client_addr, &new_client_len);
        if (new_client_fd < 0) {
            perror("accept() failed on new port");
            return -1;
        }
        return new_client_fd;
    }

    // تابع برای دریافت اطلاعات از کلاینت
    bool receiveClientInfo(int client_fd, Client_info &new_client) {
        int len = recv(client_fd, &new_client, sizeof(new_client), 0);
        if (len <= 0) {
            return false;  // خطا در دریافت اطلاعات یا اتصال قطع شده است
        }
        return true;
    }

    // تابع اصلی برای مدیریت کلاینت جدید
    void handleNewClient(int server_fd) {
        // پذیرش اتصال اولیه
        int client_fd = acceptNewClient(server_fd);
        if (client_fd < 0) return;

        // اختصاص پورت جدید به کلاینت
        int new_port = assignNewPort();

        // ایجاد سوکت جدید و bind کردن آن به پورت جدید
        int new_server_fd = create_socket(false, false);
        bind_socket(new_server_fd, new_port, false);
        listen(new_server_fd, 5);  // گوش دادن برای اتصال‌های جدید

        // ارسال پورت جدید به کلاینت
        sendNewPortToClient(client_fd, new_port);

        // بستن اتصال اولیه
        closeClientConnection(client_fd);

        // انتظار برای اتصال مجدد کلاینت به پورت جدید
        int new_client_fd = waitForClientOnNewPort(new_server_fd);
        if (new_client_fd < 0) return;

        // دریافت اطلاعات از کلاینت
        Client_info new_client;
        if (!receiveClientInfo(new_client_fd, new_client)) {
            my_print("Failed to receive client information.\n");
            closeClientConnection(new_client_fd);
            return;
        }

        // اضافه کردن کلاینت به لیست
        new_client.port = new_port;
        new_client.client_fd = new_client_fd;
        clients.push_back(new_client);

        // چاپ اطلاعات کلاینت جدید
        my_print("New client connected: ");
        my_print(new_client.username);
        my_print(" (");
        my_print(new_client.role);
        my_print(") on port ");
        my_print(std::to_string(new_port).c_str());
        my_print("\n");
    }



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
    
// ───────────── توابع کمکی ─────────────

    void prepareFdSetForServer(fd_set& read_fds, int& max_fd) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        FD_SET(udp_socket, &read_fds);
        max_fd = std::max(server_fd, udp_socket);
        for (const auto& client : clients) {
            FD_SET(client.client_fd, &read_fds);
            if (client.client_fd > max_fd) max_fd = client.client_fd;
        }
    }
    
    void handleNewConnections(fd_set& read_fds) {
        if (FD_ISSET(server_fd, &read_fds)) {
            handleNewClient(server_fd);
        }
    }
    
    void handleClientMessages(fd_set& read_fds, struct sockaddr_in& broadcast_addr) {
        char buffer[1024];
        for (auto it = clients.begin(); it != clients.end(); ) {
            if (FD_ISSET(it->client_fd, &read_fds)) {
                int len = recv(it->client_fd, buffer, sizeof(buffer) - 1, 0);
                if (len > 0) {
                    buffer[len] = '\0';
                    my_print("[TCP] Client: ");
                    my_print(buffer);
                    my_print("\n");
                
                    // پاسخ به همان کلاینت
                    send(it->client_fd, "Received your message", 21, 0);
                
                    // Broadcast پیام از طریق UDP
                    strcpy(buffer, "Hello to all clients!");
                    sendto(udp_socket, buffer, strlen(buffer), 0,
                           (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
                
                    ++it;
                } else {
                    my_print("Client disconnected.\n");
                    close(it->client_fd);
                    it = clients.erase(it);
                }
            } else {
                ++it;
            }
        }
    }
    
    void handleUdpBroadcast(fd_set& read_fds, struct sockaddr_in& broadcast_addr) {
        if (FD_ISSET(udp_socket, &read_fds)) {
            char buffer[1024] = "Hello to all clients!";
            sendto(udp_socket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        }
    }
    
    // ───────────── تابع اصلی startServer ─────────────
    
    void startServer() {
        fd_set read_fds;
        int max_fd;
    
        struct sockaddr_in broadcast_addr;
        memset(&broadcast_addr, 0, sizeof(broadcast_addr));
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_port = htons(udp_port);
        broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
        while (true) {
            prepareFdSetForServer(read_fds, max_fd);
        
            int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
            if (activity < 0) {
                perror("select() failed");
                break;
            }
        
            handleNewConnections(read_fds);
            handleClientMessages(read_fds, broadcast_addr);
            handleUdpBroadcast(read_fds, broadcast_addr);
        }
    
        for (auto& client : clients) {
            close(client.client_fd);
        }
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
