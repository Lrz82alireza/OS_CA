#include "GameManager.h"

void GameManager::handleMessage(Client_info *client_, Team *team_, const std::string& message) {

    Message msg = decodeMessage(message);

    if (team_->coder->client_fd == client_->client_fd) {
        handleCoderMessage(client_, team_, msg);
    } else if (team_->navigator->client_fd == client_->client_fd) {
        handleNavigatorMessage(client_, team_, msg);
    }
}

void GameManager::handleCoderMessage(Client_info *client, Team *team, Message &msg) {
    switch (msg.type)
    {
    case CODE_N:
        // todo: store code
        storeCode(msg.content, team);
        sendCodeToNavigator(team);
        break;
    
    case CHAT_N:
        // send message to navigator
        send(team->navigator->client_fd, msg.content.c_str(), msg.content.length(), 0);
        break;

    default:
        // invalid message
        sendInvalidMessage(client->client_fd);
        break;
    }
}

void GameManager::handleNavigatorMessage(Client_info *client, Team *team, Message &msg) {

    switch (msg.type)
    {
    case CHAT_N:
        // send message to coder
        send(team->coder->client_fd, msg.content.c_str(), msg.content.length(), 0);
        break;
    case SUBMIT_N:
        // submit
        submitCode(team, msg.content);
        break;
    default:
        // invalid message
        sendInvalidMessage(client->client_fd);
        break;
    }
}

Message GameManager::decodeMessage(const std::string& message) {
    std::string type = extractType(message);
    Message msg = {-1, ""};

    if (type == CODE_STR) {
        msg.type = CODE_N;
        msg.content = message.substr(type.length() + 1);
    } else if (type == CHAT_STR) {
        msg.type = CHAT_N;
        msg.content = message.substr(type.length() + 1);
    } else if (type == SUBMIT_STR) {
        msg.type = SUBMIT_N;
        msg.content = message.substr(type.length() + 1);
    }

    return msg;
}

void GameManager::sendQuestion()
{   
    std::string msg = "question ";
    msg += std::to_string(state + 1);
    msg += ": " + questions[state] + "\n";
    sendto(*udp_socket, msg.c_str(), msg.length(), 0, (struct sockaddr*)broadcast_addr, sizeof(*broadcast_addr));
}

void GameManager::sendInvalidMessage(int client_fd) {
    std::string msg = "Invalid message type";
    send(client_fd, msg.c_str(), msg.length(), 0);
}

void GameManager::storeCode(const std::string& code, Team *team) {
    strncpy(team->submission.code, code.c_str(), sizeof(team->submission.code) - 1);
}

void GameManager::sendCodeToNavigator(Team *team) {
    send(team->navigator->client_fd, team->submission.code, strlen(team->submission.code), 0);
}

void GameManager::submitCode(Team *team, const std::string& code) {
    strncpy(team->submission.code, code.c_str(), sizeof(team->submission.code) - 1);
    team->state.submitted[state] = true;

    std::string team_name = team->coder->username;
    team_name += " - ";
    team_name += team->navigator->username;

    strncpy(team->submission.team_name, team_name.c_str(), sizeof(team->submission.team_name) - 1);

    strncpy(team->submission.problem_id, questions[state].c_str(), sizeof(team->submission.problem_id) - 1);

    // send submission to STP
    std::string submission_msg = "-------Code submitted-------\n";
    submission_msg += "Team: " + team_name + "\n";
    submission_msg += "Problem: " + questions[state] + "\n";
    submission_msg += "Code: " + code + "\n";
    sendMsgToTeam(team, submission_msg);
}