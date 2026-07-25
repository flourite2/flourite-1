// InputManager.h

#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

// 새로운 아키텍처의 절대 경로 참조
#include "Engine/Core/Math/Vector2D.h"
#include "Engine/Modules/Input/InputParser.h"

// 이동 키와 방향 벡터를 묶어주는 내부 구조체
struct MoveBinding {
    sf::Keyboard::Key key;
    Vector2D direction;
};

class InputManager {
private:
    std::vector<MoveBinding> moveContainer;
    std::unordered_map<sf::Keyboard::Key, std::vector<std::function<void()>>> eventRegistry;

public:
    // 생성자: 파싱된 inputTable과 이동 방향을 받는다.
    InputManager(std::unordered_map<std::string, InputMapData>& inputTable);

    // 이동 키 입력 시 이동 방향을 업데이트. 입력 방향은 정규화가 된 벡터여야 함
    void UpdateMoveDirection(Vector2D& moveDirection);

    // 단발성 이벤트(점프, 공격 등) 등록 및 처리
    void RegisterEvent(sf::Keyboard::Key key, std::function<void()> callback);
    void handleEvent(sf::Keyboard::Key pressedKey) const;
};