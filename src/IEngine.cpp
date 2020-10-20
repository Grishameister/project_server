#include "IEngine.h"

size_t Event::next_id = 1;

Event::Event(std::shared_ptr<ClientSocket> cl, std::string& data, event status):
                client(std::move(cl)), _data(data), _status(status), id(next_id++) {
}

Event::Event(std::shared_ptr<ClientSocket> cl, std::string& data, event status, std::function<void (int)> cb):
                client(std::move(cl)), _data(data), _status(status), callback(cb), id(next_id++) {
}
