// InputManager.cpp

#include "Engine/Modules/Input/InputManager.h"
#include "Engine/Modules/Input/InputKeys.h"

InputManager::InputManager(std::unordered_map<std::string, InputMapData>& inputTable) {
    moveContainer = {
        { InputKeys::ToSFMLKey(inputTable["Move_Up"].input).value_or(sf::Keyboard::Key::Unknown),    Vector2D(0, -1) },
        { InputKeys::ToSFMLKey(inputTable["Move_Down"].input).value_or(sf::Keyboard::Key::Unknown),  Vector2D(0, 1)  },
        { InputKeys::ToSFMLKey(inputTable["Move_Left"].input).value_or(sf::Keyboard::Key::Unknown),  Vector2D(-1, 0) },
        { InputKeys::ToSFMLKey(inputTable["Move_Right"].input).value_or(sf::Keyboard::Key::Unknown), Vector2D(1, 0)  }
    };
}

void InputManager::UpdateMoveDirection(Vector2D& moveDirection) {
    for (const auto& binding : moveContainer) {
        float isPressed = static_cast<float>(sf::Keyboard::isKeyPressed(binding.key));
        moveDirection += binding.direction * isPressed;
    }
}

void InputManager::RegisterEvent(sf::Keyboard::Key key, std::function<void()> callback) {
    eventRegistry[key].push_back(callback);
}

void InputManager::handleEvent(sf::Keyboard::Key pressedKey) const {
    auto it = eventRegistry.find(pressedKey);
    if (it != eventRegistry.end()) {
        for (const auto& callback : it->second) {
            callback();
        }
    }
}