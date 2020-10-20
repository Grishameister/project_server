#ifndef PROJECT_SERVER_USER_H
#define PROJECT_SERVER_USER_H

#include <string>
#include <memory>
#include <functional>

#include "IUser.h"

using AsyncFunction = std::function<void(std::shared_ptr<ClientSocket>, std::string&, std::function<void(int)>)>;

template<class AsyncRead, class AsyncWrite>
class User: public IUser, public std::enable_shared_from_this<User<AsyncRead, AsyncWrite>> {
private:
    std::string buffer_;
    size_t session_id_;

    AsyncRead m_async_read_;
    AsyncWrite m_async_write_;

    std::function<int (const std::string&, std::shared_ptr<User>)> m_switch_;

    void ReadHandler(int error) {
        auto usr = this->shared_from_this();
        m_switch_(buffer_, usr);
    }
    void WriteHandler(int error) {
        read();
    }
public:
    User(std::shared_ptr<ClientSocket> sock): IUser(sock) {}
    User(std::shared_ptr<ClientSocket> sock, AsyncRead async_read, AsyncWrite async_write,
            std::function<int (const std::string&, std::shared_ptr<User>)> switch_f):
            IUser(sock), m_async_read_(async_read), m_async_write_(async_write), m_switch_(switch_f){}
    void write(const std::string& message) override {
        buffer_ = message;
        auto cb = [usr = this->shared_from_this()](int error) {
            usr->WriteHandler(error);
        };
        m_async_write_(socket_, std::ref(buffer_), cb);
    }
    void read() override {
        auto cb = [usr = this->shared_from_this()](int error){
            usr->ReadHandler(error);
        };
        m_async_read_(socket_, std::ref(buffer_), cb);
    }

    void SetSessionId(size_t session) {
        session_id_ = session;
    }
    size_t GetSessionId() {
        return session_id_;
    }

    ~User() = default;
};

using SwitchFunction = std::function<int (const std::string&, std::shared_ptr<User<AsyncFunction, AsyncFunction>>)>;
using UserPtr = std::shared_ptr<User<AsyncFunction, AsyncFunction>>;


#endif //PROJECT_SERVER_USER_H
