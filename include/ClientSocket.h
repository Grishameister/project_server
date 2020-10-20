#ifndef PROJECT_SERVER_CLIENTSOCKET_H
#define PROJECT_SERVER_CLIENTSOCKET_H


#include "Socket.h"

class ClientSocket: public Socket {
public:
    explicit ClientSocket(int socket, bool option = true): Socket(socket) {
        if (option) {
            SetNonBlocking(true);
        }
    }

    ~ClientSocket() = default;
    int Receive(std::string& msg);
    int SendMsg(const std::string& msg);

};


#endif //PROJECT_SERVER_CLIENTSOCKET_H
