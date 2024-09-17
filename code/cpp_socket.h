#ifndef __CPP_SOCKET_H
#define __CPP_SOCKET_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <utility>
#include <iostream>
#include <cstring>
#include <format>


class Socket {
public:
    Socket(int family, int type);
    Socket(int fd, std::string addr = "", int port = -1);
    ~Socket();

    bool operator==(const Socket& sock);

    bool Bind(const std::string& host, int port);

    bool Listen(int backlog = 1024);

    bool Connect(const std::string&, int);

    Socket Accept(void);

    int Send(const std::string& messsge);

    std::string Recv(size_t);

    // 默认为阻塞
    bool SetBlocking(bool op = true);

    // 一般情况,level = SOL_SOCKET
    bool SetSockopt(int level, int optname, int value);

    void Close(void);

    std::string GetIp(void);

    int GetPort(void);

private:
    int sockfd_;
    std::string ip_;
    int port_;
};

#endif