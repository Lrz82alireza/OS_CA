#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <set>

#include "SHARED.h"
#include "GameManager.h"


#define UDP_PORT 8081

#define ROLE_CODER_STR "coder"
#define ROLE_NAVIGATOR_STR "navigator"

#define ERR_USERNAME_STR "ERR: Username already exists."
#define ERR_ROLE_STR "ERR: Invalid role."

#define START_STR "start"

std::set<int> assigned_ports;
int BASE_PORT = 5000;

class Server {
private:
    struct sockaddr_in broadcast_addr;

    int port_cntr = 0;

    int server_fd;
    int stp_port;
    
    int udp_socket;
    int udp_port = UDP_PORT;

    int start_flag = -1;

    std::vector<Client_info*> clients;
    std::vector<Team*> teams;
    
    GameManager* gameManager;

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
        // بررسی پورت‌های قبلی و اختصاص مجدد در صورت امکان
        for (int port = BASE_PORT; port < BASE_PORT + clients.size(); ++port) {
            if (assigned_ports.find(port) == assigned_ports.end()) {  // اگر پورت قبلاً اختصاص داده نشده است
                assigned_ports.insert(port);
                return port;
            }
        }

        // اگر هیچ پورتی از قبل آزاد نبود، یک پورت جدید اختصاص بده
        int new_port = BASE_PORT + clients.size();
        assigned_ports.insert(new_port);
        return new_port;
    }

    // تابع برای ارسال پورت جدید به کلاینت
    void sendNewPortToClient(int client_fd, int new_port) {
        char port_msg[10];
        sprintf(port_msg, "%d", new_port);
        send(client_fd, port_msg, strlen(port_msg), 0);
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

// _____________ Check Client Info _____________

    int HasUniqueUsername(Client_info new_client) {
        for (int i = 0; i < clients.size(); i++) {
            if (strcmp(clients[i]->username, new_client.username) == 0) {
                return -1;
            }
        }
        return 1;
    }

    int HasValidRole(Client_info new_client) {
        if (strcmp(new_client.role, ROLE_CODER_STR) == 0) {
            return 1;
        }
        if (strcmp(new_client.role, ROLE_NAVIGATOR_STR) == 0) {
            return 1;
        }
        return -1;
    }

    int checkClientInfo(Client_info new_client) {
        if (HasUniqueUsername(new_client) == -1) {
            send(new_client.client_fd, ERR_USERNAME_STR, strlen(ERR_USERNAME_STR), 0);
            return -1;
        }
        if (HasValidRole(new_client) == -1) {
            send(new_client.client_fd, ERR_ROLE_STR, strlen(ERR_ROLE_STR), 0);
            return -1;
        }
        return 1;
    }
// _____________ Check Client Info _____________
    int addNewClient(Client_info new_client) {
        if (checkClientInfo(new_client) == -1) {
            return -1;
        }
        Client_info * ptr = new Client_info(new_client);
        
        Team *team = findTeamByClientName(teams, new_client.username);
        if (team != nullptr) {
            my_print("Client recconected\n");
            handleClientReconnection(teams, clients, ptr);
        } else {
            new_client.has_teammate = false;
        }
        
        clients.push_back(ptr);
        return clients.size();
    }

    // تابع اصلی برای مدیریت کلاینت جدید
    // void handleNewClient(int server_fd) {
    //     // پذیرش اتصال اولیه
    //     int client_fd = acceptNewClient(server_fd);
    //     if (client_fd < 0) return;

    //     // اختصاص پورت جدید به کلاینت
    //     int new_port = assignNewPort();

    //     // ایجاد سوکت جدید و bind کردن آن به پورت جدید
    //     int new_server_fd = create_socket(false, false);
    //     bind_socket(new_server_fd, new_port, false);
    //     listen(new_server_fd, 5);  // گوش دادن برای اتصال‌های جدید

    //     // ارسال پورت جدید به کلاینت
    //     sendNewPortToClient(client_fd, new_port);

    //     // بستن اتصال اولیه
    //     close(client_fd);

    //     // انتظار برای اتصال مجدد کلاینت به پورت جدید
    //     my_print("just a new one\n");
    //     int new_client_fd = waitForClientOnNewPort(new_server_fd);
    //     if (new_client_fd < 0) return;

    //     // دریافت اطلاعات از کلاینت
    //     my_print("just a new one\n");
    //     Client_info new_client;
    //     if (!receiveClientInfo(new_client_fd, new_client)) {
    //         my_print("Failed to receive client information.\n");
    //         closeClientConnection(assigned_ports, new_client_fd, new_port);
    //         return;
    //     }
    //     new_client.port = new_port;
    //     new_client.client_fd = new_client_fd;

    //     my_print("just a new one\n");
    //     if (addNewClient(new_client) == -1) {
    //         send(new_client.client_fd, "ERR: Invalid information", 25, 0);
    //         closeClientConnection(assigned_ports, new_client.client_fd, new_port);
    //         return;
    //     }

    //     // چاپ اطلاعات کلاینت جدید
    //     my_print("New client connected: ");
    //     my_print(new_client.username);
    //     my_print(" (");
    //     my_print(new_client.role);
    //     my_print(") on port ");
    //     my_print(std::to_string(new_port).c_str());
    //     my_print("\n");
    // }

    void handleNewClient(int server_fd) {
        // پذیرش اتصال اولیه
        int client_fd = acceptNewClient(server_fd);
        if (client_fd < 0) return;
    
        // اختصاص پورت جدید به کلاینت
        int new_port = assignNewPort();
    
        // ایجاد سوکت جدید و bind کردن آن به پورت جدید
        int new_server_fd = create_socket(false, false);
        bind_socket(new_server_fd, new_port, false);
    
        if (listen(new_server_fd, 5) < 0) {  // بررسی خطا در listen
            perror("Failed to listen on new port");
            close(new_server_fd);
            close(client_fd);
            return;
        }
    
        // ارسال پورت جدید به کلاینت
        sendNewPortToClient(client_fd, new_port);
    
        // بستن اتصال اولیه
        close(client_fd);
    
        // انتظار برای اتصال مجدد کلاینت به پورت جدید
        my_print("Waiting for client on new port...\n");
        int new_client_fd = waitForClientOnNewPort(new_server_fd);
        
        // بستن سوکت سرور جدید بعد از قبول اتصال
        close(new_server_fd);
    
        if (new_client_fd < 0) return;
    
        // دریافت اطلاعات از کلاینت
        Client_info new_client;
        if (!receiveClientInfo(new_client_fd, new_client)) {
            my_print("Failed to receive client information.\n");
            closeClientConnection(assigned_ports, new_client_fd, new_port);
            return;
        }
        new_client.port = new_port;
        new_client.client_fd = new_client_fd;
    
        if (addNewClient(new_client) == -1) {
            send(new_client.client_fd, "ERR: Invalid information", 25, 0);
            closeClientConnection(assigned_ports, new_client.client_fd, new_port);
            return;
        }
    
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
            FD_SET(client->client_fd, &read_fds);
            if (client->client_fd > max_fd) max_fd = client->client_fd;
        }
    }
    
    void handleNewConnections(fd_set& read_fds) {
        if (FD_ISSET(server_fd, &read_fds)) {
            handleNewClient(server_fd);
        }
    }
    
    void handleClientMessages(fd_set& read_fds) {
        char buffer[1024];
        for (auto it = clients.begin(); it != clients.end(); ) {
            Client_info *client = *it;


            if (FD_ISSET(client->client_fd, &read_fds)) {
                int len = recv(client->client_fd, buffer, sizeof(buffer) - 1, 0);
                if (len > 0) {
                    buffer[len] = '\0';
                    my_print("[TCP] Client ");
                    my_print(client->username);
                    my_print(": ");
                    my_print(buffer);
                    my_print("\n");
                
                    // پاسخ به همان کلاینت
                    send(client->client_fd, "Received your message", 21, 0);
                
                    // Broadcast پیام از طریق UDP
                    strcpy(buffer, "Hello to all clients!");
                    sendto(udp_socket, buffer, strlen(buffer), 0,
                           (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
                
                    ++it;
                } else {
                    handleClientDisconnection(assigned_ports, teams, clients, *it);
                }
            } else {
                ++it;
            }
        }
    }
    
    void handleUdpBroadcast(fd_set& read_fds) {
        if (FD_ISSET(udp_socket, &read_fds)) {
            char buffer[1024] = "Hello to all clients!";
            sendto(udp_socket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        }
    }
    
    void startGame() {
        if (start_flag == -1) {
            start_flag = 1;
            gameManager = new GameManager(&broadcast_addr, &server_fd, &stp_port, &udp_socket, &udp_port, clients, teams);
            my_print("Game started.\n");
        }
    }

    void handleKeyboardInput(fd_set& read_fds) {
        char buffer[1024];
        int len = 0;
    
        // بررسی ورودی از کیبورد
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (len > 0) {
                buffer[len] = '\0';

                // پردازش ورودی
                if (strcmp(buffer, "start\n") == 0) {
                    startGame();
                    return;
                }
                if (strcmp(buffer, "team\n") == 0) {
                    int count = 0;
                    my_print("Teams: \n");
                    for (auto team : teams) {
                        my_print("Team ");
                        my_print(to_string(count).c_str());
                        my_print(": ");
                        my_print(team->coder->username);
                        my_print(" - ");
                        my_print(team->navigator->username);
                        my_print("\n");
                        count++;
                    }
                }
                if (strcmp(buffer, "clients\n") == 0) {
                    my_print("Clients: \n");
                    for (auto client : clients) {
                        my_print(client->username);
                        my_print(" (");
                        my_print(client->role);
                        my_print(")\n");
                    }
                }
            }
        }
    }

    void pairUpClientsMessage(Client_info *client, Client_info *other) {
        char buffer[1024];
        strcpy(buffer, "You are paired with ");
        strcat(buffer, other->username);
        strcat(buffer, " as your teammate.");
        send(client->client_fd, buffer, strlen(buffer), 0);
    }

    void pairUpClients() {
        for (auto client : clients) {            
            if (client->has_teammate) {
                continue;
            }
            for (auto other : clients) {
                if (other->has_teammate) {
                    continue;
                }
                if (other->role == client->role) {
                    continue;
                }

                Team *team = new Team();
                if (client->role == ROLE_CODER_STR) {
                    team->coder = client;
                    team->navigator = other;
                } else {
                    team->coder = other;
                    team->navigator = client;
                }

                client->has_teammate = true;
                other->has_teammate = true;

                pairUpClientsMessage(client, other);
                pairUpClientsMessage(other, client);

                my_print("Team created: ");
                my_print(team->coder->username);
                my_print(" - ");
                my_print(team->navigator->username);
                my_print("\n");

                teams.push_back(team);
            }
        }
    }

    int handleGameMessege(Client_info *Client, Team *team, fd_set& read_fd) {

        char buffer[1024];
        
        if (FD_ISSET(Client->client_fd, &read_fd)) {
            int len = recv(Client->client_fd, buffer, sizeof(buffer) - 1, 0);
            if (len > 0) {
                buffer[len] = '\0';
                gameManager->handleMessage(Client, team, buffer);
            } else {
                return -1;
            }
        }
        return 1;
    }

    void handleGameMessages(fd_set& read_fds) {

        for (auto team : teams) {
            if (handleGameMessege(team->coder, team, read_fds) == -1) {
                // todo: remove client from teams
                handleClientDisconnection(assigned_ports, teams, clients, team->coder);
                my_print("Client ");
                my_print(team->coder->username);
                my_print(" disconnected.\n");
            }
            if (handleGameMessege(team->navigator, team, read_fds) == -1) {
                // todo: remove client from teams
                handleClientDisconnection(assigned_ports, teams, clients, team->navigator);
                my_print("Client ");
                my_print(team->navigator->username);
                my_print(" disconnected.\n");
            }
        }
    }

    // ───────────── تابع اصلی startServer ─────────────
    
    void startServer() {
        fd_set read_fds;
        int max_fd;

        // تنظیمات برای ارسال Broadcast
        memset(&broadcast_addr, 0, sizeof(broadcast_addr));
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_port = htons(udp_port);
        broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
        while (true) {
            // handle time
            if (gameManager->handleTime() == END_GAME) {
                break;
            }

            prepareFdSetForServer(read_fds, max_fd);

            // اضافه کردن STDIN_FILENO به مجموعه فایل‌ها
            FD_SET(STDIN_FILENO, &read_fds);
            max_fd = std::max(max_fd, STDIN_FILENO); 

            // تنظیم timeout برای select
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000000; // 0.5 ثانیه

            int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
            if (activity < 0) {
                my_print("select() failed");
                break;
            }
        
            // I/O Processing
            handleKeyboardInput(read_fds);
        
            handleNewConnections(read_fds);
            if (start_flag == -1) {
                pairUpClients();
                handleClientMessages(read_fds);
            } else {
                handleGameMessages(read_fds);
            }
            // handleUdpBroadcast(read_fds);
        }
    }

    
    ~Server() {
        if (server_fd != -1) close(server_fd);
        delete(gameManager);
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
