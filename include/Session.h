#ifndef PROJECT_SERVER_SESSION_H
#define PROJECT_SERVER_SESSION_H

#include <set>


#include "User.h"
#include "Data.h"



class Session {
private:
    static size_t next_id_;
    size_t id_;
    std::set<UserPtr> users_;
    size_t started_;
public:
    explicit Session(UserPtr user);

    Error AddUserInSession(UserPtr user);
    //EraseState EraseUser(UserPtr user);

    void NotifyUsers(const std::string& message);

    bool IsFull();

    size_t GetId();
};

#endif //PROJECT_SERVER_SESSION_H
