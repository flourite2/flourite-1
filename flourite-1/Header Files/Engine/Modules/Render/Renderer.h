// Renderer.h

#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

class Renderer {
private:
    sf::RenderWindow window;
    int maxFPS;
    unsigned int resolutionX;
    unsigned int resolutionY;
    int drawCalls = 0;
    int textureSwaps = 0;
    const sf::Texture* lastTexture = nullptr; // 포인터로 수정 (원래 코드 반영)

public:
    Renderer(int fps, unsigned int resX, unsigned int resY);
    bool IsOpen() const;
    void Close();
    void Clear(sf::Color color = sf::Color::Black);
    void Draw(const sf::Drawable& drawable, const sf::Texture* currentTexture = nullptr);
    void Display();
    std::optional<sf::Event> PollEvent();
    int GetAndResetDrawCalls();
    int GetAndResetTextureSwaps();
	void SetView(const sf::View& view);
    sf::View GetView() const {
        return window.getView();
	}
};