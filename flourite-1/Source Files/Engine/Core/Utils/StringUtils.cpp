// StringUtils.h

#include "Engine/Core/Utils/StringUtils.h"
#include <algorithm>
#include <sstream>
#include <iostream>

// 문자열의 양 끝 공백을 제거
std::string StringUtils::Trim(const std::string& str) {
    if (str.empty()) return "";

    const std::string border = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(border);
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(border);
    return str.substr(start, (end - start + 1));
}

// 문자열을 특정 문자로 분리
std::vector<std::string> StringUtils::Split(const std::string& str, char delim) {
    std::vector<std::string> output;
    std::stringstream ss(str);
    std::string splitted;

    while (std::getline(ss, splitted, delim)) {
        output.push_back(splitted);
    }

    return output;
       
}

// 문자열을 int로 변경 가능하다면 변경. target은 변환 대상, targetName은 대상을 식별하기 위한 이름.
std::optional<int> StringUtils::TryParseInt(const std::string& target, const std::string& targetName) {
    try {
        size_t pos = 0;
        int parsed = std::stoi(target, &pos);

        if (pos == target.length()) {
            return parsed;
        }
        else {
            std::cerr << "정확한 int 값이 아님. " << targetName << "의 값: " << target << std::endl;
            return std::nullopt;
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "int로 변환할 수 없음. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
    catch (const std::out_of_range& e) {
        std::cerr << "int의 범위를 초과함. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
    catch (const std::exception& e) {
        std::cerr << "TryParseInt() 실행 중 예외 발생. " << targetName << "의 값: " << target << ", 예외: " << e.what() << std::endl;
        return std::nullopt;
    }
    catch (...) {
        std::cerr << "TryParseInt() 실행 중 예기치 못한 예외 발생. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
}

// 문자열을 float로 변경 가능하다면 변경. target은 변환 대상, targetName은 대상을 식별하기 위한 이름.
std::optional<float> StringUtils::TryParseFloat(const std::string& target, const std::string& targetName) {
    try {
        size_t pos = 0;
        float parsed = std::stof(target, &pos);

        if (pos == target.length()) {
            return parsed;
        }
        else {
            std::cerr << "정확한 float 값이 아님. " << targetName << "의 값: " << target << std::endl;
            return std::nullopt;
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "float로 변환할 수 없음. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
    catch (const std::out_of_range& e) {
        std::cerr << "float의 범위를 초과함. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
    catch (const std::exception& e) {
        std::cerr << "TryParseFloat() 실행 중 예외 발생. " << targetName << "의 값: " << target << ", 예외: " << e.what() << std::endl;
        return std::nullopt;
    }
    catch (...) {
        std::cerr << "TryParseFloat() 실행 중 예기치 못한 예외 발생. " << targetName << "의 값: " << target << std::endl;
        return std::nullopt;
    }
}
