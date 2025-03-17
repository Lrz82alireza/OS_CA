#include "GameManager.h"

void GameManager::handleMessage(Client_info *client_, Team *team_, const std::string& message) {

    int result;
    if (result = checkTime() == NEXT_TURN) {
        handleEndTurn();
        return;
    }

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
        msg.content = "[TCP] Coder: \n" + msg.content;
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
        msg.content = "[TCP] Navigator: \n" + msg.content;
        send(team->coder->client_fd, msg.content.c_str(), msg.content.length(), 0);
        break;
    case SUBMIT_N:
        // submit
        submitCode(team);
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
    } else if (type == CHAT_STR) {
        msg.type = CHAT_N;
    } else if (type == SUBMIT_STR) {
        msg.type = SUBMIT_N;
    } else {
        std::string tmp = "Invalid message type: ";
        tmp += type;
        tmp += "\n";
        my_print(tmp.c_str());
    }
    msg.content = message.substr(type.length() + 1);

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
    std::string msg = "-------Code received-------\n";
    msg += team->submission.code;
    send(team->navigator->client_fd, msg.c_str(), msg.length(), 0);
}

void GameManager::submitCode(Team *team) {
    if (team->state.submitted[state]) {
        sendMsgToTeam(team, "You have already submitted this question.");
        return;
    }

    team->state.submitted[state] = true;

    std::string team_name = team->coder->username;
    team_name += " - ";
    team_name += team->navigator->username;

    strncpy(team->submission.team_name, team_name.c_str(), sizeof(team->submission.team_name) - 1);

    strncpy(team->submission.problem_id, questions[state].c_str(), sizeof(team->submission.problem_id) - 1);

    team->state.time_submitted[state] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - gameStartTime).count();

    // send submission to STP
    std::string submission_msg = "-------Code submitted-------\n";
    submission_msg += "Team: " + team_name + "\n";
    submission_msg += "Problem: " + questions[state] + "\n";
    submission_msg += "Time: ";
    submission_msg += std::to_string(team->state.time_submitted[state]);
    submission_msg += "\n";
    submission_msg += "Code: ";
    submission_msg += team->submission.code;
    submission_msg += "\n";
    sendMsgToTeam(team, submission_msg);
}

int GameManager::checkTime() {
    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - gameStartTime).count();
    
    if (diff >= TIME_LIMIT) {
        gameStartTime = now;
        return NEXT_TURN;
    }
    return IN_TURN;
}

int GameManager::handleEndTurn() {

    scoreTeams();
    sendResults();
    
    state++;
    if (state == MAX_STATE) {
        // end game
        std::string msg = "---------Game over---------\n";
        sendto(*udp_socket, msg.c_str(), msg.length(), 0, (struct sockaddr*)broadcast_addr, sizeof(*broadcast_addr));
        return 0;
    }
    

    return 1;
}

int GameManager::scoreTeams() {
    for (auto team : teams) {
        int score = 0;
        score = calculateScore(team, state);
        team->score[state] = score;
    }
    return 1;
}

int GameManager::sendResults()
{
    std::string msg = "Results: \n";
    for (auto team : teams) {
        msg += team->coder->username;
        msg += " - ";
        msg += team->navigator->username;
        msg += ":\n";
        for (int i = 0; i < state; i++) {
            msg += "\t Question ";
            msg += std::to_string(i + 1);
            msg += ": ";
            msg += std::to_string(team->score[i]);
            msg += "\n";
        }
    }
    sendto(*udp_socket, msg.c_str(), msg.length(), 0, (struct sockaddr*)broadcast_addr, sizeof(*broadcast_addr));
    return 1;
}

int GameManager::calculateScore(const Team *team, int state)
{
    int score = 0;
    if (sendCodeToEvaluation(team, state) < 0) {
        return 0;
    }

    score += SCORES[state];

    score += calculateBonus(team, state);

    return score;
}

int GameManager::calculateBonus(const Team *team, int state)
{
    int score = 0;
    if (team->state.time_submitted[state] < TIME_LIMIT / 2) {
        score += SCORES[state] / 2;
    } else if (team->state.time_submitted[state] < TIME_LIMIT) {
        score += SCORES[state] / 5;
    }
    return score;
}

int GameManager::sendCodeToEvaluation(const Team *team, int state)
{
    std::string msg = problem_ids[state] + '\n';
    msg += team->submission.code;

    send(evaluation_fd, msg.c_str(), msg.length(), 0);
    

    char buffer[16];
    int len = recv(evaluation_fd, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        if (strcmp(buffer, PASS) == 0) {
            return 1;
        }
    }
    return -1;
}
