#include "GameManager.h"

void GameManager::handleMessage(Client_info *client_, Team *team_, const std::string& message) {

    if (message == "MOVE") {
        // handleMove(client.client_fd);
    } else if (message == "CHAT") {
        // handleChat(client.client_fd);
    } else {
        // todo
    }
}

void GameManager::handleMove(int client_fd) {
    send(client_fd, "Move received", 13, 0);
}

void GameManager::handleChat(int client_fd) {
    send(client_fd, "Chat received", 13, 0);
}

void GameManager::sendQuestion()
{   
    std::string msg = "question ";
    msg += std::to_string(state + 1);
    msg += ": " + questions[state] + "\n";
    sendto(*udp_socket, msg.c_str(), msg.length(), 0, (struct sockaddr*)broadcast_addr, sizeof(*broadcast_addr));
}
