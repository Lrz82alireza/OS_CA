#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>

#include "SHARED.h"

class GameManager {
public:
    GameManager(int* server_fd, int* stp_port, int* udp_socket, int* udp_port, std::vector<Client_info*> clients, std::vector<Team*> teams)
        : server_fd(server_fd), stp_port(stp_port), udp_socket(udp_socket), udp_port(udp_port) {
            this->clients = clients;
            this->teams = teams;
        };

    void handleMessage(Client_info *client, const std::string& message);

private:
    int* server_fd;
    int* stp_port;

    int* udp_socket;
    int* udp_port;

    std::vector<Client_info*> clients;
    std::vector<Team*> teams;

    void handleMove(int client_fd);
    void handleChat(int client_fd);
};

#endif // GAME_MANAGER_H
