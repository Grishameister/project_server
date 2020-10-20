#ifndef PROJECT_SERVER_DATA_H
#define PROJECT_SERVER_DATA_H

#include <msgpack.hpp>
#include <string>

enum class Route {
    CreateSession = 1,
    JoinSession,
    WriteData,
    ReadData,
    CreateUser,
    Enter
};

MSGPACK_ADD_ENUM(Route)

enum class Error {
    Success = 1,
    NotFound,
    KeyExist,
    Full,
    UserExist,
    InvalidLogIn,
    Started,
    Wait,
    NotValidMap,
    EndGame
};

MSGPACK_ADD_ENUM(Error)

struct AuthData {
    std::string login_;
    std::string password_;

    MSGPACK_DEFINE_MAP(login_, password_)

    AuthData() = default;
    AuthData(const std::string& login, const std::string& password): login_(login), password_(password) {}
};

struct Date {
    int d;
    int m;
    int y;
    MSGPACK_DEFINE_MAP(d, m, y)
    Date() = default;
};

struct Counters {
    size_t people;
    size_t objects;
    MSGPACK_DEFINE_MAP(people, objects)
    Counters() = default;
};


struct DataRequest {
    int user_id_;
    int session_id_;
    Route type_;
    AuthData data_;
    Counters counters_;
    Date date_;

    MSGPACK_DEFINE_MAP(user_id_, session_id_, type_, data_, counters_, date_)

    DataRequest() = default;
    DataRequest(const AuthData& data, Route type): type_(type), data_(data) {}
    friend std::ostream& operator<<(std::ostream& os, const DataRequest& rq);
};

inline std::ostream& operator<<(std::ostream& os, const DataRequest& rq) {
    os << "id: " << rq.user_id_ << std::endl << rq.date_.d << "-" << rq.date_.m << "-" << rq.date_.y << std::endl
    << "people: " << rq.counters_.people << std::endl << "objects: " << rq.counters_.objects << std::endl;
    return os;
}

struct DataResponse {
    int user_id_;
    int session_id_;
    Route type_;
    Error error_;
    std::vector<Counters> counters_;
    Date date_;

    MSGPACK_DEFINE_MAP(user_id_, session_id_, type_, error_, counters_, date_)
    DataResponse() = default;

    friend std::ostream& operator<<(std::ostream& os, const DataResponse& rq);
};

inline std::ostream& operator<<(std::ostream& os, const DataResponse& rq) {
    os << "id: " << rq.user_id_ << std::endl << rq.date_.d << "-" << rq.date_.m << "-" << rq.date_.y << std::endl;
    for (auto& c: rq.counters_) {
       os << "people: " << c.people << std::endl << "objects: " << c.objects << std::endl;
    }
    return os;
}

#endif //PROJECT_SERVER_DATA_H
