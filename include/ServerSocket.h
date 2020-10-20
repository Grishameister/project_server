#ifndef PROJECT_SERVER_SERVERSOCKET_H
#define PROJECT_SERVER_SERVERSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string>
#include <cstring>
#include <memory>
#include <arpa/inet.h>
#include <fcntl.h>

#include "Socket.h"
#include "ClientSocket.h"

class ServerSocket: public Socket {
public:
    ServerSocket(const std::string& host, uint32_t port) noexcept(false);
    std::shared_ptr<ClientSocket> AcceptClient();
};


#endif //PROJECT_SERVER_SERVERSOCKET_H
