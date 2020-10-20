#include <iostream>
#include <thread>

#include "EngineServer.h"

EngineServer::EngineServer(std::string host, uint32_t port):
                            Server(std::move(host), port) {
    engine_ = std::make_shared<Engine>();
    parser_ = std::make_shared<Parser>();
    session_manager_ = std::make_shared<SessionManager>();
    access_object_ = std::make_shared<DBAccess>();
    handler_ = std::make_shared<HandlerManager>(*session_manager_, *access_object_);
}

int EngineServer::SwitchAction(const std::string& message, UserPtr user) {
    std::string response;
    std::shared_ptr<DataResponse> rp = std::make_shared<DataResponse>();
    if (message.empty()) {
        need_close_.push_back(user->GetClient()->GetFd());
        clients_.erase(user->GetId());
        return 1;
    }

    std::unique_ptr<DataRequest> rq = parser_->Deserialize(message);

    rp->type_ = rq->type_;
    rp->user_id_ = user->GetId();

    handler_->Handle(rq, user, rp);
    parser_->Serialize(rp, response);
    user->write(response);
    return 0;
}


void EngineServer::DoAccept() {
    while (true) {
        std::shared_ptr<ClientSocket> new_client = m_socket->AcceptClient();

        if (new_client && new_client->GetFd() > -1) {
            std::cerr<< "new client's sd is " << new_client->GetFd() << std::endl;

            auto read = std::bind(&EngineServer::AsyncRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            auto write = std::bind(&EngineServer::AsyncWrite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            auto switch_f = std::bind(&EngineServer::SwitchAction, this, std::placeholders::_1, std::placeholders::_2);

            auto user = std::make_shared<User<AsyncFunction, AsyncFunction>>(new_client, read, write, switch_f);
            clients_.insert({user->GetId(), user});
            user->read();
        }
        usleep(1000);
    }
}

void EngineServer::AsyncRead(std::shared_ptr<ClientSocket> client, std::string& message, std::function<void(int)> callback) {
    Event event = Event(client, message, event::WantRead, callback);

    auto result = std::find_if(want_read_events_.begin(), want_read_events_.end(), [&event](Event& ev){
        return event.client->GetFd() == ev.client->GetFd();});

    if (result == want_read_events_.end()) {
        want_read_events_.push_back(event);
    }
}

void EngineServer::AsyncWrite(std::shared_ptr<ClientSocket> client, std::string& message, std::function<void(int)> callback) {
    Event event = Event(client, message, event::WantWrite, callback);
    work_events_.push(event);
}

void EngineServer::run() {
    auto ManageFunction = [this]() {
        engine_->ManageClients(want_read_events_, work_events_, m_want_read_events_, m_work_events_);
    };
    auto ProcessFunction = [this]() {
        Process();
    };
    std::thread manage_thread(ManageFunction);

    std::vector<std::thread> process_threads;

    process_threads.reserve(3);
    for (int i = 0; i < 3; i++) {
        process_threads.emplace_back(ProcessFunction);
    }

    DoAccept();

    manage_thread.join();
    for (auto& t: process_threads) {
        t.join();
    }
}

void EngineServer::Process() {
    while (true) {
        std::string msg;
        std::shared_ptr<ClientSocket> cl = std::make_shared<ClientSocket>(4, false);
        Event event (cl, msg, event::WantRead);
        {
            std::unique_lock<std::mutex> lock(m_work_events_);
            if (work_events_.empty()) {
                lock.unlock();
                usleep(1000);
                continue;
            }
            event = work_events_.front();
            work_events_.pop();
        }
        if (event._status == event::WantRead) {
            event.client->Receive(msg);
            event._data.get().assign(msg);
        } else {
            event.client->SendMsg(event._data.get());
        }
        event.callback(errno);
    }
}
