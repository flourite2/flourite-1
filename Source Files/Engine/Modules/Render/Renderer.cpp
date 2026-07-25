// Renderer.cpp

#include "Engine/Modules/Render/Renderer.h"

Renderer::Renderer(int fps, unsigned int resX, unsigned int resY)
    : maxFPS(fps), resolutionX(resX), resolutionY(resY),
    window(sf::VideoMode({ resX, resY }), "Game Window")
{
    window.setFramerateLimit(fps);
}

bool Renderer::IsOpen() const {
    return window.isOpen();
}

void Renderer::Close() {
    window.close();
}

void Renderer::Clear(sf::Color color) {
    window.clear(color);
}

void Renderer::Draw(const sf::Drawable& drawable, const sf::Texture* currentTexture) {
    drawCalls++;

    if (currentTexture != nullptr && currentTexture != lastTexture) {
        textureSwaps++;
        lastTexture = currentTexture;
    }
    window.draw(drawable);
}

void Renderer::Display() {
    window.display();
}

std::optional<sf::Event> Renderer::PollEvent() {
    return window.pollEvent();
}

int Renderer::GetAndResetDrawCalls() {
    int current = drawCalls;
    drawCalls = 0;
    return current;
}

int Renderer::GetAndResetTextureSwaps() {
    int current = textureSwaps;
    textureSwaps = 0;
    return current;
}

void Renderer::SetView(const sf::View& view) {
    window.setView(view);
}