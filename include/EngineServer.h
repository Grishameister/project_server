#ifndef PROJECT_SERVER_ENGINESERVER_H
#define PROJECT_SERVER_ENGINESERVER_H

#include <utility>
#include <queue>
#include <unordered_map>

#include "Server.h"
#include "Parser.h"
#include "Engine.h"
#include "SessionManager.h"
#include "Handler.h"
#include "../DBAccess/include/DBAccess.h"

class EngineServer: public Server {
private:
    std::shared_ptr<IEngine> engine_;
    std::shared_ptr<IParser> parser_;
    std::shared_ptr<SessionManager> session_manager_;
    std::shared_ptr<DBAccess> access_object_;
    std::shared_ptr<HandlerManager> handler_;

    std::mutex m_want_read_events_;
    std::mutex m_work_events_;

    void AsyncRead(std::shared_ptr<ClientSocket> client, std::string& message, std::function<void(int)> callback);
    void AsyncWrite(std::shared_ptr<ClientSocket> client, std::string& message, std::function<void(int)> callback);

    void DoAccept();
    void Process();

    std::vector<int> need_close_;
protected:
    std::unordered_map<int, UserPtr> clients_;
    std::vector<Event> want_read_events_;
    std::queue<Event> work_events_;
public:
    EngineServer(std::string host, uint32_t port);
    ~EngineServer() {
        for (auto& sock: need_close_) {
            close(sock);
        }
    }
    void run() override;
    int SwitchAction(const std::string& message, UserPtr user);
};


#endif //PROJECT_SERVER_ENGINESERVER_H
