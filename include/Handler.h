#ifndef PROJECT_SERVER_HANDLER_H
#define PROJECT_SERVER_HANDLER_H

#include "Data.h"
#include "SessionManager.h"
#include "Parser.h"
#include "../DBAccess/include/DBAccess.h"

class Handler {
public:
    Handler(SessionManager& session_manager, DBAccess& access_obj);
    virtual bool CanHandle(std::unique_ptr<DataRequest>& rq) = 0;
    virtual void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) = 0;
protected:
    std::reference_wrapper<SessionManager> session_manager_;
    std::reference_wrapper<DBAccess> access_obj_;
    std::shared_ptr<IParser> parser_;
};

class CSHandler: public Handler {
public:
    CSHandler(SessionManager& session_manager, DBAccess& access_obj);
    bool CanHandle(std::unique_ptr<DataRequest>& rq) override;
    void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) override;
};

class JSHandler: public Handler {
public:
    JSHandler(SessionManager& session_manager, DBAccess& access_obj);
    bool CanHandle(std::unique_ptr<DataRequest>& rq) override;
    void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) override;
};

class WDHandler: public Handler {
public:
    WDHandler(SessionManager& session_manager, DBAccess& access_obj);
    bool CanHandle(std::unique_ptr<DataRequest>& rq) override;
    void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) override;
};

class RDHandler: public Handler {
public:
    RDHandler(SessionManager& session_manager, DBAccess& access_obj);
    bool CanHandle(std::unique_ptr<DataRequest>& rq) override;
    void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp) override;
};

class HandlerManager {
private:
    std::vector<std::shared_ptr<Handler>> handlers_;
public:
    HandlerManager(SessionManager& sm, DBAccess& access_obj);
    void Handle(std::unique_ptr<DataRequest>& rq, UserPtr user, std::shared_ptr<DataResponse> rp);
};

#endif //PROJECT_SERVER_HANDLER_H