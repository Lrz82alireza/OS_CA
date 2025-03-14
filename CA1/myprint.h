// myprint.h

#ifndef MYPRINT_H
#define MYPRINT_H

#include <string>
#include <sstream>

void my_print(const char* str);
std::string to_string(int value);
std::string to_string(double value);
std::string to_string(const char* value);

int create_socket(bool is_udp, bool is_broadcast);
void bind_socket(int sock_fd, int port, bool is_udp);
void connect_socket(int sock_fd, const char* ip, int port);

#endif // MYPRINT_H
