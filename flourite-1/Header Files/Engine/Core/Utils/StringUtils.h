// StringUtils.h

#pragma once
#include <string>
#include <vector>
#include <optional>

namespace StringUtils {
    // 문자열 양쪽 공백 제거 함수
    std::string Trim(const std::string& str);
    std::vector<std::string> Split(const std::string& str, char delim);

    // 안전한 Parse 함수
    std::optional<int> TryParseInt(const std::string& target, const std::string& targetName);
    std::optional<float> TryParseFloat(const std::string& target, const std::string& targetName);
}