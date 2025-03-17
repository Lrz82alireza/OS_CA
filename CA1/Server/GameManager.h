#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>

#include "SHARED.h"

#define SERVER_IP "127.0.0.1"

#define Q1 "def add_numbers(a, b):\n"
#define Q2 "def reverse_string(s):\n"
#define Q3 "def is_palindrome(s):\n"

#define P_ID1 "add_numbers"
#define P_ID2 "reverse_string" 
#define P_ID3 "is_palindrome"

#define END_GAME 3
#define NEXT_TURN 10
#define IN_TURN 2
#define TIME_LIMIT 60

#define MAX_STATE 3


const std::string questions[3] = {Q1, Q2, Q3};
const std::string problem_ids[3] = {P_ID1, P_ID2, P_ID3};
const float SCORES[3] = {1.0, 3.0, 5.0};

class GameManager {
public:
    GameManager(sockaddr_in* broadcast_addr, int* server_fd, int* stp_port, int* udp_socket, int* udp_port, std::vector<Client_info*> clients, std::vector<Team*> teams)
        : broadcast_addr(broadcast_addr), server_fd(server_fd), stp_port(stp_port), udp_socket(udp_socket), udp_port(udp_port) {
            this->clients = clients;
            this->teams = teams;

            gameStartTime = std::chrono::steady_clock::now();
            evaluation_fd = createEvaluationSocket(SERVER_IP);
            sendQuestion();
        };

        ~GameManager() {
            if (evaluation_fd != -1) close(evaluation_fd);
        }
        
    void handleMessage(Client_info *client, Team *team, const std::string& message);
    int handleTime();

private:
    int state = 0;

    bool halfTimeAnnounced = false;

    std::chrono::steady_clock::time_point gameStartTime;

    int evaluation_fd;

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

    void submitCode(Team *team);

    void sendInvalidMessage(int client_fd);

    int checkTime();
    int handleEndTurn();

    int scoreTeams();
    int sendResults();

    float calculateScore(const Team* team, int state);
    float calculateBonus(const Team* team, int state);
    int sendCodeToEvaluation(const Team* team, int state);

};

#endif // GAME_MANAGER_H
