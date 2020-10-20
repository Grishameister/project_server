#include "ServerSocket.h"

#include "Socket.h"

ServerSocket::ServerSocket(const std::string& host, uint32_t port) noexcept(false) {
    sock_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd_ <= 0) {
        throw std::runtime_error("socket: " + std::string(strerror(errno)));
    }

    int yes = 1;
    if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        Close();
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host.c_str());
    server_addr.sin_port = htons(port);

    if (bind(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        Close();
        throw std::runtime_error("bind: " + std::string(strerror(errno)));
    }
    listen(GetFd(), 1000);
}


std::shared_ptr<ClientSocket> ServerSocket::AcceptClient() {
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t client_len = sizeof(client);

    int client_sd = accept(GetFd(), (struct sockaddr*)&client, &client_len);
    if (client_sd == -1) {
        return std::shared_ptr<ClientSocket>();
    }
    return std::make_shared<ClientSocket>(client_sd);
}
