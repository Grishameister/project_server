#ifndef PROJECT_SERVER_IPARSER_H
#define PROJECT_SERVER_IPARSER_H

#include <string>
#include <sstream>

#include "Data.h"


class IParser {
public:
    IParser():  buffer("") {}
    virtual ~IParser() = default;
    virtual std::unique_ptr<DataRequest> Deserialize(const std::string& request) = 0;
    virtual void Serialize(std::shared_ptr<DataResponse> rp, std::string& response) = 0;
private:
    std::string buffer;
};

#endif //PROJECT_SERVER_IPARSER_H
