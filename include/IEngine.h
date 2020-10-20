#ifndef PROJECT_SERVER_IENGINE_H
#define PROJECT_SERVER_IENGINE_H
#include <vector>
#include <functional>
#include <queue>
#include <mutex>

#include "ClientSocket.h"

enum class event {
    WantWrite,
    Wait,
    WantRead
};

struct Event {
    Event(std::shared_ptr<ClientSocket> cl, std::string& data, event status);

    Event(std::shared_ptr<ClientSocket> cl, std::string& data, event status, std::function<void (int)> cb);

    Event& operator= (const Event& rhs) {
        client = rhs.client;
        _data = std::ref(rhs._data);
        _status = rhs._status;
        callback = rhs.callback;
        id = rhs.id;
        return *this;
    }

    std::shared_ptr<ClientSocket> client;
    std::reference_wrapper<std::string> _data;
    event _status;
    std::function<void (int)> callback;
    size_t id;

private:
    static size_t next_id;
};


class IEngine {
public:
    IEngine(){}
    virtual void ManageClients(std::vector<Event>& wantReadEvents, std::queue<Event>& workEvents, std::mutex& mWantReadEvents, std::mutex& mWorkEvents) = 0;
    virtual ~IEngine() = default;
};


#endif //PROJECT_SERVER_IENGINE_H
