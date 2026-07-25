// InputKeys.h

#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <optional>
#include <unordered_map>

namespace InputKeys {
    // 문자열(예: "W", "Up")을 SFML 키보드 열거형으로 반환합니다.
    std::optional<sf::Keyboard::Key> ToSFMLKey(const std::string& keyStr);
}