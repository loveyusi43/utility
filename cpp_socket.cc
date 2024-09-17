#include "cpp_socket.h"

Socket::Socket(int family, int type) {
    sockfd_ = ::socket(family, type, IPPROTO_TCP);
    if (sockfd_ < 0) {
        std::cout << "创建套接字失败" << std::endl;
        throw std::runtime_error("创建监听套接字失败");
    }
}

Socket::Socket(int fd, std::string addr, int port) : sockfd_{fd}, ip_{addr}, port_{port} {
}

Socket::~Socket() {
    //Close();
}

bool Socket::operator==(const Socket& sock) {
    return this->sockfd_ == sock.sockfd_;
}

bool Socket::Bind(const std::string& host, int port)  {
    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;

    if (host.empty())
        sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        sockaddr.sin_addr.s_addr = inet_addr(host.c_str());

    sockaddr.sin_port = htons(port);

    if (::bind(sockfd_, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "绑定失败" << std::endl;
        throw std::runtime_error("监听套接字绑定失败");
        return false;
    }

    ip_ = host;
    port_ = port;
    return true;
}

bool Socket::Listen(int backlog) {
    if (::listen(sockfd_, backlog) < 0) {
        std::cout << "监听失败" << std::endl;
        throw std::runtime_error("套接字监听失败");
        return false;
    }

    return true;
}

bool Socket::Connect(const std::string& addr, int port) {
    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    sockaddr.sin_port = htons(port);

    if (::connect(sockfd_, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "连接失败" << std::endl;
        return false;
    }

    ip_ = addr;
    port_ = port;
    return true;
}

Socket Socket::Accept(void) {
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);

    int connfd = ::accept(sockfd_, (struct sockaddr*)&client_address, &client_addrlen);
    if (connfd < 0) {
        std::cout << "客户端连接失败" << std::endl;
        throw std::runtime_error("客户端连接失败");
        return Socket(-1);
    }

    // 获取客户端的IP地址和端口号
    char client_ip[INET_ADDRSTRLEN];  // IPv4地址长度
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_address.sin_port);
    std::cout << "Client connected - IP: " << client_ip << ", Port: " << client_port << std::endl;
    return Socket(connfd, client_ip, client_port);
}

int Socket::Send(const std::string& data) {
    return ::send(sockfd_, data.c_str(), data.size(), 0);
}

std::string Socket::Recv(size_t buffer_size) {
    char buffer[buffer_size];
    memset(buffer, 0, sizeof(buffer));

    // 调用 recv 函数从套接字中读取数据

    ssize_t bytesReceived = recv(sockfd_, buffer, buffer_size, 0);
    if (bytesReceived < 0) {
        std::cout << std::format("\033[31m错误代码: {}, 错误信息: {}\033[0m", errno, strerror(errno));
        throw std::runtime_error("Recv出错");
    }

    return std::string(buffer, bytesReceived);
}

void Socket::Close(void) {
    if (sockfd_ > 0) {
        ::close(sockfd_);
    }
}

bool Socket::SetBlocking(bool op) {
    if (op)
        return false;

    int flag = fcntl(sockfd_, F_GETFL, 0);
    if (flag < 0) {
        std::cout << "设置非阻塞失败" << std::endl;
        return false;
    }

    flag |= O_NONBLOCK;
    if (fcntl(sockfd_, F_SETFL, flag) < 0) {
        std::cout << "设置非阻塞失败" << std::endl;
        return false;
    }

    return true;
}

bool Socket::SetSockopt(int level, int optname, int value) {
    if (setsockopt(sockfd_, level, optname, &value, sizeof(value)) < 0) {
        std::cout << "套接字设置失败" << std::endl;
        return false;
    }

    return true;
}

std::string Socket::GetIp(void) {
    return ip_;
}

int Socket::GetPort(void) {
    return port_;
}