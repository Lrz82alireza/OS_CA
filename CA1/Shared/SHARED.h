// SHARED.h

#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

struct Client_info {
    char username[50];
    char role[20];
    int port;
    int client_fd;
    bool has_teammate = false;
};

struct State
{
    bool submitted[3] = {false, false, false};
};

struct Submission
{
    char team_name[50] = "";
    char problem_id[20] = "";
    char code[2000] = "";
};


struct Team
{
    Client_info* coder = nullptr;
    Client_info* navigator = nullptr;
    int score[3] = {0, 0, 0};
    State state;
    Submission submission;
};


void my_print(const char* str);
std::string to_string(int value);
std::string to_string(double value);
std::string to_string(const char* value);

int create_socket(bool is_udp, bool is_broadcast);
void bind_socket(int sock_fd, int port, bool is_udp);
void connect_socket(int sock_fd, const char* ip, int port);

std::string extractType(const std::string& input);
void sendMsgToTeam(Team* team, const std::string& msg);

#endif // SHARED_H
