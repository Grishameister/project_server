#include "Session.h"

size_t Session::next_id_ = 1;

Session::Session(UserPtr user): id_(next_id_++), started_(0) {
    users_.insert(user);
}


Error Session::AddUserInSession(UserPtr user) {
    if (IsFull()) {
        return Error::Full;
    }
    if (users_.insert(user).second) {
        return Error::Success;
    }
    return Error::UserExist;
}


void Session::NotifyUsers(const std::string& message) {
    for (auto& user: users_) {
        user->write(message);
    }
}




/*EraseState Session::EraseUser(UserPtr user) {
    users_.erase(user);
    if (started_ == 2) {
        int winner_id = -1;
        started_ = 0;
        return EraseState(winner_id);
    }
    return EraseState();
}*/

bool Session::IsFull() {
    return users_.size() >= 2;
}

size_t Session::GetId() {
    return id_;
}
