// StringUtils.h

#include "Engine/Core/Utils/StringUtils.h"
#include <algorithm>
#include <sstream>
#include <iostream>

std::string StringUtils::Trim(const std::string& str) {
    if (str.empty()) return "";

    const std::string border = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(border);
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(border);
    return str.substr(start, (end - start + 1));
}

std::vector<std::string> StringUtils::Split(const std::string& str, char delim) {
    std::vector<std::string> output;
    std::stringstream ss(str);
    std::string splitted;

    while (std::getline(ss, splitted, delim)) {
        output.push_back(splitted);
    }

    return output;
       
}
