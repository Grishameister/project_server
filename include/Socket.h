#ifndef PROJECT_SERVER_SOCKET_H
#define PROJECT_SERVER_SOCKET_H
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string>
#include <cstring>
#include <memory>
#include <arpa/inet.h>
#include <fcntl.h>

class Socket {
protected:
    int sock_fd_;
public:
    Socket() noexcept(false) : sock_fd_(-1){}
    explicit Socket(int socket): sock_fd_(socket){};

    virtual ~Socket(){

    }

    void SetNonBlocking(bool option);
    int GetFd();
    void Close();
};

#endif //PROJECT_SERVER_SOCKET_H
