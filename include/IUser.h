#ifndef PROJECT_SERVER_IUSER_H
#define PROJECT_SERVER_IUSER_H
#include <string>

#include "ClientSocket.h"

class IUser {
private:
    static size_t next_id_;
protected:
    std::shared_ptr<ClientSocket> socket_;
    std::string name_;
    size_t id_;
public:
    explicit IUser(std::shared_ptr<ClientSocket> sock);
    virtual ~IUser() = default;

    void SetName(const std::string& username);
    virtual void write(const std::string& message) = 0;
    virtual void read() = 0;
    size_t GetId();
    std::shared_ptr<ClientSocket> GetClient();
};


#endif //PROJECT_SERVER_IUSER_H
