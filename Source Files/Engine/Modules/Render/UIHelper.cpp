// UIHelper.cpp

#include "Engine/Modules/Render/UIHelper.h"

namespace {
#include <SFML/Graphics.hpp>

    // 9-Patch UI를 그려주는 마법의 헬퍼 함수
    void DrawNineSlice(sf::RenderWindow& window, const sf::Texture& texture, sf::FloatRect bounds, float cornerSize) {
        sf::Vector2f texSize((float)texture.getSize().x, (float)texture.getSize().y);

        // SFML 3.0 생성자 규격: { {x, y}, {width, height} }
        sf::IntRect texRects[9] = {
            { {0, 0}, {(int)cornerSize, (int)cornerSize} },
            { {(int)cornerSize, 0}, {(int)(texSize.x - cornerSize * 2), (int)cornerSize} },
            { {(int)(texSize.x - cornerSize), 0}, {(int)cornerSize, (int)cornerSize} },

            { {0, (int)cornerSize}, {(int)cornerSize, (int)(texSize.y - cornerSize * 2)} },
            { {(int)cornerSize, (int)cornerSize}, {(int)(texSize.x - cornerSize * 2), (int)(texSize.y - cornerSize * 2)} },
            { {(int)(texSize.x - cornerSize), (int)cornerSize}, {(int)cornerSize, (int)(texSize.y - cornerSize * 2)} },

            { {0, (int)(texSize.y - cornerSize)}, {(int)cornerSize, (int)cornerSize} },
            { {(int)cornerSize, (int)(texSize.y - cornerSize)}, {(int)(texSize.x - cornerSize * 2), (int)cornerSize} },
            { {(int)(texSize.x - cornerSize), (int)(texSize.y - cornerSize)}, {(int)cornerSize, (int)cornerSize} }
        };

        sf::FloatRect drawRects[9] = {
            { {bounds.position.x, bounds.position.y}, {cornerSize, cornerSize} },
            { {bounds.position.x + cornerSize, bounds.position.y}, {bounds.size.x - cornerSize * 2, cornerSize} },
            { {bounds.position.x + bounds.size.x - cornerSize, bounds.position.y}, {cornerSize, cornerSize} },

            { {bounds.position.x, bounds.position.y + cornerSize}, {cornerSize, bounds.size.y - cornerSize * 2} },
            { {bounds.position.x + cornerSize, bounds.position.y + cornerSize}, {bounds.size.x - cornerSize * 2, bounds.size.y - cornerSize * 2} },
            { {bounds.position.x + bounds.size.x - cornerSize, bounds.position.y + cornerSize}, {cornerSize, bounds.size.y - cornerSize * 2} },

            { {bounds.position.x, bounds.position.y + bounds.size.y - cornerSize}, {cornerSize, cornerSize} },
            { {bounds.position.x + cornerSize, bounds.position.y + bounds.size.y - cornerSize}, {bounds.size.x - cornerSize * 2, cornerSize} },
            { {bounds.position.x + bounds.size.x - cornerSize, bounds.position.y + bounds.size.y - cornerSize}, {cornerSize, cornerSize} }
        };

        sf::Sprite sprite(texture);
        for (int i = 0; i < 9; ++i) {
            sprite.setTextureRect(texRects[i]);
            sprite.setPosition(drawRects[i].position);
            sprite.setScale({
                drawRects[i].size.x / (float)texRects[i].size.x,
                drawRects[i].size.y / (float)texRects[i].size.y
                });
            window.draw(sprite);
        }
    }
}