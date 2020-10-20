#include "IUser.h"
size_t IUser::next_id_ = 1;

IUser::IUser(std::shared_ptr<ClientSocket> sock): socket_(sock), id_(next_id_++){

}

size_t IUser::GetId() {
    return id_;
}

void IUser::SetName(const std::string& username) {
    name_ = username;
}

std::shared_ptr<ClientSocket> IUser::GetClient() {
    return socket_;
}

