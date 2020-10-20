#include "SessionManager.h"

size_t SessionManager::CreateSession(UserPtr user) {
    SessionPtr new_session = std::make_shared<Session>(user);
    sessions_.insert({new_session->GetId(), new_session});
    return new_session->GetId();
}

Error SessionManager::AddUserInSession(UserPtr user, size_t id) {
    auto it = sessions_.find(id);
    if (it == sessions_.end()) {
        return Error::NotFound;
    }
    return it->second->AddUserInSession(user);
}

bool SessionManager::DeleteSession(size_t id) {
    return sessions_.erase(id);
}



void SessionManager::NotifySession(const std::string &message, size_t id) {
    auto iterator = sessions_.find(id);
    if (iterator != sessions_.end()) {
        iterator->second->NotifyUsers(message);
    }
}

/*std::shared_ptr<EraseState> SessionManager::EraseUser(UserPtr user, size_t session_id) {
    auto iterator = sessions_.find(session_id);
    if (iterator == sessions_.end()) {
        return nullptr;
    }

    EraseState result = iterator->second->EraseUser(user);
    std::shared_ptr<EraseState> result_ptr = std::make_shared<EraseState>(result.started_, result.winner_id_);
    return result_ptr;
}*/
