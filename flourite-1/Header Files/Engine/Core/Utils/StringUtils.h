// StringUtils.h

#pragma once
#include <string>
#include <vector>

namespace StringUtils {
    // 문자열 양쪽 공백 제거 함수
    std::string Trim(const std::string& str);
    std::vector<std::string> Split(const std::string& str, char delim);
}