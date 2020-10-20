#include <unordered_set>
#include "include/DBAccess.h"

bool DBAccess::WriteData(const std::string& key, const std::string& value) {
    return redis.sadd(key, value);
}

bool DBAccess::ReadData(const std::string& key, std::vector<std::string>& set) {
    redis.smembers(key, std::inserter(set, set.begin()));
    return true;
}
