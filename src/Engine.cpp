#include <sys/poll.h>
#include <iostream>

#include "Engine.h"


void Engine::DeleteEvents(std::vector<Event>& want_read_events, std::vector<size_t>& need_delete,
        std::mutex& m_want_read_events) {
    {
        std::unique_lock<std::mutex> lock(m_want_read_events);
        for (auto& id: need_delete) {
            want_read_events.erase(std::remove_if(want_read_events.begin(), want_read_events.end(),
                    [&id](Event& event){return event.id == id;}), want_read_events.end());
        }

    }
    {
        std::unique_lock<std::mutex> lock(m_need_delete_);
        need_delete.clear();
    }
}

void Engine::ManageClients(std::vector<Event>& want_read_events, std::queue<Event>& work_events,
        std::mutex& m_want_read_events, std::mutex& m_work_events) {

    while(true) {
        if (want_read_events.empty()) {
            usleep (1000);
            continue;
        }
        struct pollfd fds[1024];
        std::vector<size_t> need_delete;

        int i = 0;
        for (auto& client: want_read_events) {
            fds[i].fd = client.client->GetFd();
            fds[i].events = POLLIN;
            fds[i].revents = 0;
            ++i;
        }

        int result = poll(fds, want_read_events.size(), 50);

        if (result == 0) {
            usleep(1000);
            continue;
        }
        if (result == -1) {
            return;
        }

        size_t size = want_read_events.size();

        for (size_t k = 0; k < size; k++) {
            if (fds[k].revents == 0) {
                continue;
            }

            if (fds[k].revents & POLLHUP) {
                std::unique_lock<std::mutex> lock_n(m_need_delete_);
                need_delete.push_back(want_read_events[k].id);
            } else if (fds[k].revents & POLLIN) {
                {
                    std::unique_lock<std::mutex> lock_w(m_work_events);
                    work_events.push(want_read_events[k]);
                }
                {
                    std::unique_lock<std::mutex> lock_n(m_need_delete_);
                    need_delete.push_back(want_read_events[k].id);
                }
            } else if (fds[k].revents & POLLNVAL) {
                std::unique_lock<std::mutex> lock_n(m_need_delete_);
                need_delete.push_back(want_read_events[k].id);
            }
        }
        DeleteEvents(want_read_events, need_delete, m_want_read_events);
    }
}
