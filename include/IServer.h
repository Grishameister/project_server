#ifndef PROJECT_SERVER_ISERVER_H
#define PROJECT_SERVER_ISERVER_H

#include <string>
#include <utility>

class IServer {
private:
    std::string m_host;
    uint32_t m_port;
public:
    IServer(std::string  host, uint32_t port) : m_host(std::move(host)), m_port(port) {}
    virtual ~IServer() = default;
    virtual void run() = 0;
};


#endif //PROJECT_SERVER_ISERVER_H
