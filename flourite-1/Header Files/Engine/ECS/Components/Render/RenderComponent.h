// RenderComponent.h

#pragma once
#include <SFML/Graphics.hpp>

struct RenderComponent {
    const sf::Texture* texture = nullptr;
    sf::IntRect textureRect;
    sf::Vector2f origin;
    sf::Color color = sf::Color::White;
    int layer = 0;

    // ★ 신규 추가: 애니메이션 독립 재생을 위한 변수들
    std::string animKey = ""; // 예: "girl_sprite"
    float animTimer = 0.0f;
    int animTick = 0;
};