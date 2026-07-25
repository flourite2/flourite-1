// TileMap.h

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class TileMap : public sf::Drawable, public sf::Transformable {
private:
    sf::VertexArray m_vertices;
    const sf::Texture* m_tileset = nullptr; // ★ 변경됨: 원본을 가지지 않고 포인터만 가짐!

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    // mapHeight 매개변수 추가
    bool Load(const sf::Texture* tilesetTexture, int tileSize, int sheetColumns, int mapWidth, int mapHeight, float scale, const std::vector<int>& mapData);
};