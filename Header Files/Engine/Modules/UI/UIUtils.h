// UIUtils.h

#pragma once
#include "Engine/Modules/Render/Renderer.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

namespace UIUtils {
    // 인라인으로 처리하여 cpp 파일 없이 h 파일만으로 작동하게 구성
    inline void DrawNineSlice(Renderer* renderer, const sf::Texture& texture, sf::FloatRect bounds, float cornerSize, sf::Color color) {
        sf::Vector2f texSize((float)texture.getSize().x, (float)texture.getSize().y);

        // 모서리
        float cX = std::min(cornerSize, (texSize.x / 2.0f) - 1.0f);
        float cY = std::min(cornerSize, (texSize.y / 2.0f) - 1.0f);
        if (cX <= 0 || cY <= 0) return;

        // 중앙
        float midTexW = texSize.x - cX * 2.0f;
        float midTexH = texSize.y - cY * 2.0f;

        // 중앙이 늘어날 크기
        float midDrawW = bounds.size.x - cX * 2.0f;
        float midDrawH = bounds.size.y - cY * 2.0f;

        sf::IntRect texRects[9] = {
            { {0, 0}, {(int)cX, (int)cY} }, // 좌상단
            { {(int)cX, 0}, {(int)midTexW, (int)cY} },  
            { {(int)(texSize.x - cX), 0}, {(int)cX, (int)cY} },
            { {0, (int)cY}, {(int)cX, (int)midTexH} },
            { {(int)cX, (int)cY}, {(int)midTexW, (int)midTexH} },
            { {(int)(texSize.x - cX), (int)cY}, {(int)cX, (int)midTexH} },
            { {0, (int)(texSize.y - cY)}, {(int)cX, (int)cY} },
            { {(int)cX, (int)(texSize.y - cY)}, {(int)midTexW, (int)cY} },
            { {(int)(texSize.x - cX), (int)(texSize.y - cY)}, {(int)cX, (int)cY} }
        };

        sf::FloatRect drawRects[9] = {
            { {bounds.position.x, bounds.position.y}, {cX, cY} },
            { {bounds.position.x + cX, bounds.position.y}, {midDrawW, cY} },
            { {bounds.position.x + bounds.size.x - cX, bounds.position.y}, {cX, cY} },
            { {bounds.position.x, bounds.position.y + cY}, {cX, midDrawH} },
            { {bounds.position.x + cX, bounds.position.y + cY}, {midDrawW, midDrawH} },
            { {bounds.position.x + bounds.size.x - cX, bounds.position.y + cY}, {cX, midDrawH} },
            { {bounds.position.x, bounds.position.y + bounds.size.y - cY}, {cX, cY} },
            { {bounds.position.x + cX, bounds.position.y + bounds.size.y - cY}, {midDrawW, cY} },
            { {bounds.position.x + bounds.size.x - cX, bounds.position.y + bounds.size.y - cY}, {cX, cY} }
        };

        sf::Sprite sprite(texture);
		sprite.setColor(color);
        for (int i = 0; i < 9; ++i) {
            if (texRects[i].size.x <= 0 || texRects[i].size.y <= 0) continue;
            sprite.setTextureRect(texRects[i]);
            sprite.setPosition(drawRects[i].position);
            sprite.setScale({
                drawRects[i].size.x / (float)texRects[i].size.x,
                drawRects[i].size.y / (float)texRects[i].size.y
                });
            renderer->Draw(sprite);
        }
    }
}