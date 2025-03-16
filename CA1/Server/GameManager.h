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

#define CODE_STR "/code"
#define CHAT_STR "/chat"
#define SUBMIT_STR "/submit"

#define CODE_N 1
#define CHAT_N 2
#define SUBMIT_N 3


const std::string questions[3] = {Q1, Q2, Q3};

struct Message
{
    int type;
    std::string content;
};


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
    void handleCoderMessage(Client_info *client, Team *team, Message &message);
    void handleNavigatorMessage(Client_info *client, Team *team, Message &message);
    void sendQuestion();

    void storeCode(const std::string& code, Team *team);
    void sendCodeToNavigator(Team *team);

    void submitCode(Team *team, const std::string& code);

    Message decodeMessage(const std::string& message);
    void sendInvalidMessage(int client_fd);

    
};

#endif // GAME_MANAGER_H
