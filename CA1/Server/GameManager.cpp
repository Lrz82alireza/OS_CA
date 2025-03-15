#include "GameManager.h"

void GameManager::handleMessage(Client_info *client, const std::string& message) {

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
