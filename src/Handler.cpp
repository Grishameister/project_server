#include "Handler.h"

Handler::Handler(SessionManager& session_manager, DBAccess& access_obj) : session_manager_(session_manager), access_obj_(access_obj) {
    parser_ = std::make_shared<Parser>();
}

JSHandler::JSHandler(SessionManager& session_manager, DBAccess& access_obj) : Handler(session_manager, access_obj) {
}

bool JSHandler::CanHandle(std::unique_ptr<DataRequest>& rq) {
    return rq->type_ == Route::JoinSession;
}

void JSHandler::Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) {
    std::string response;
    int id = rq->session_id_;
    Error result = session_manager_.get().AddUserInSession(user, id);
    if (result == Error::Success) {
        user->SetSessionId(id);
    }
    rp->error_ = result;
    parser_->Serialize(rp, response);
    session_manager_.get().NotifySession(response, id);
}

CSHandler::CSHandler(SessionManager &session_manager, DBAccess& access_obj) : Handler(session_manager, access_obj) {
}

bool CSHandler::CanHandle(std::unique_ptr<DataRequest>& rq) {
    return rq->type_ == Route::CreateSession;
}

void CSHandler::Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) {
    std::string response;
    size_t id = session_manager_.get().CreateSession(user);
    user->SetSessionId(id);
    rp->session_id_ = id;
}


WDHandler::WDHandler(SessionManager &session_manager, DBAccess& access_obj) : Handler(session_manager, access_obj) {
}

bool WDHandler::CanHandle(std::unique_ptr<DataRequest> &rq) {
    return rq->type_ == Route::WriteData;
}

void WDHandler::Handle(std::unique_ptr<DataRequest> &rq, UserPtr user, std::shared_ptr<DataResponse> rp) {
    std::stringstream key;
    key << rq->date_.d << rq->date_.m << rq->date_.y;
    std::cout << key.str() << std::endl;
    std::cout << *rq;
    std::stringstream value;
    msgpack::pack(value, *rq);
    if (!access_obj_.get().WriteData(key.str(), value.str())) {
        rp->error_ = Error::KeyExist;
    }
    rp->error_ = Error::Success;
}

RDHandler::RDHandler(SessionManager& session_manager, DBAccess& access_obj) : Handler(session_manager, access_obj) {
}

bool RDHandler::CanHandle(std::unique_ptr<DataRequest> &rq) {
    return rq->type_ == Route::ReadData;
}

void RDHandler::Handle(std::unique_ptr<DataRequest> &rq, UserPtr user, std::shared_ptr<DataResponse> rp) {
    std::stringstream key;
    key << rq->date_.d << rq->date_.m << rq->date_.y;
    std::cout << key.str() << std::endl;
    std::vector<std::string> set;
    access_obj_.get().ReadData(key.str(), set);
    if (set.empty()) {
        rp->error_ =  Error::NotFound;
    } else {
        rp->error_ = Error::Success;
    }
    for (auto& s: set) {
        std::shared_ptr<DataRequest> rq1 = std::make_shared<DataRequest>();
        msgpack::object_handle oh = msgpack::unpack(s.data(), s.size());
        msgpack::object obj = oh.get();
        obj.convert(*rq1);
        rp->counters_.push_back(rq1->counters_);
    }

    rp->date_ = rq->date_;
    std::cout << *rp << std::endl;
}


HandlerManager::HandlerManager(SessionManager& sm, DBAccess& access_obj) {
    std::shared_ptr<Handler> js_handler = std::make_shared<JSHandler>(sm, access_obj);
    handlers_.push_back(js_handler);
    std::shared_ptr<Handler> cs_handler = std::make_shared<CSHandler>(sm, access_obj);
    handlers_.push_back(cs_handler);
    std::shared_ptr<Handler> wd_handler = std::make_shared<WDHandler>(sm, access_obj);
    handlers_.push_back(wd_handler);
    std::shared_ptr<Handler> rd_handler = std::make_shared<RDHandler>(sm, access_obj);
    handlers_.push_back(rd_handler);
}

void HandlerManager::Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) {
    for (auto& handler: handlers_) {
        if (handler->CanHandle(rq)) {
            handler->Handle(rq, user, rp);
            break;
        }
    }
}

