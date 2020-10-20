#ifndef PROJECT_SERVER_SERVER_H
#define PROJECT_SERVER_SERVER_H

#include "IServer.h"
#include "ServerSocket.h"

class Server: public IServer{
protected:
    std::shared_ptr<ServerSocket> m_socket;
public:
    Server(const std::string& host, uint32_t port) : IServer(host, port) {
        m_socket = std::make_shared<ServerSocket>(host, port);
    }

    void run() override;
};


#endif //PROJECT_SERVER_SERVER_H
