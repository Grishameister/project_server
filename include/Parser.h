#ifndef PROJECT_SERVER_PARSER_H
#define PROJECT_SERVER_PARSER_H

#include "IParser.h"
#include "Session.h"

class Parser : public IParser {
public:
    Parser():IParser(){}
    std::unique_ptr<DataRequest> Deserialize(const std::string& request) override;
    void Serialize(std::shared_ptr<DataResponse> rp, std::string& response) override;
};

#endif //PROJECT_SERVER_PARSER_H
