#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>

#include "SHARED.h"

#define Q1 "def add_numbers(a, b):\n"
#define Q2 "def reverse_string(s):\n"
#define Q3 "def is_palindrome(s):\n"

const std::string questions[3] = {Q1, Q2, Q3};

class GameManager {
public:
    GameManager(sockaddr_in* broadcast_addr, int* server_fd, int* stp_port, int* udp_socket, int* udp_port, std::vector<Client_info*> clients, std::vector<Team*> teams)
        : broadcast_addr(broadcast_addr), server_fd(server_fd), stp_port(stp_port), udp_socket(udp_socket), udp_port(udp_port) {
            this->clients = clients;
            this->teams = teams;

            sendQuestion();
        };

    void handleMessage(Client_info *client, Team *team, const std::string& message);

private:
    int state = 0;

    sockaddr_in* broadcast_addr;

    int* server_fd;
    int* stp_port;

    int* udp_socket;
    int* udp_port;

    std::vector<Client_info*> clients;
    std::vector<Team*> teams;

    void handleMove(int client_fd);
    void handleChat(int client_fd);
    void sendQuestion();
};

#endif // GAME_MANAGER_H
