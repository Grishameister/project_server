#ifndef PROJECT_SERVER_ENGINE_H
#define PROJECT_SERVER_ENGINE_H

#include <utility>

#include "IEngine.h"


class Engine: public IEngine {
private:
    std::mutex m_need_delete_;
public:
    Engine(): IEngine() {}
    void ManageClients(std::vector<Event>& want_read_events, std::queue<Event>& work_events,
            std::mutex& m_want_read_events, std::mutex& m_work_events) override;
    void DeleteEvents(std::vector<Event>& want_read_events, std::vector<size_t>& need_delete, std::mutex& m_want_read_events);
};


#endif //PROJECT_SERVER_ENGINE_H
