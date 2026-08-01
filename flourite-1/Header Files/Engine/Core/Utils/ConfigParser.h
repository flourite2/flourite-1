// ConfigParser.h

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace ConfigParser {
    extern std::unordered_map<std::string, std::string> settings;

    void LoadConfig(const std::string& filepath);  // 여러 번 호출 가능, settings에 누적

    int         GetInt(const std::string& key, int defaultValue);
    float       GetFloat(const std::string& key, float defaultValue);
    std::string GetString(const std::string& key, const std::string& defaultValue);
    std::vector<std::string> GetStringList(const std::string& key,
        const std::string& defaultValue = "");
}