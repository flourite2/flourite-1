// TileMap.cpp

#include "Game/Maps/TileMap.h"
#include <iostream>

// 인자로 파일 경로(std::string) 대신 텍스처 포인터를 받습니다.
bool TileMap::Load(const sf::Texture* tilesetTexture, int tileSize, int sheetColumns, int mapWidth, int mapHeight, float scale, const std::vector<int>& mapData) {

    m_tileset = tilesetTexture; // 포인터만 연결! (loadFromFile 삭제)
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    auto addVertex = [&](float px, float py, float tx, float ty) {
        sf::Vertex v;
        v.position = sf::Vector2f(px, py);
        v.texCoords = sf::Vector2f(tx, ty);
        m_vertices.append(v);
        };

    // ★ 맵 1장의 전체 타일 개수
    int layerSize = mapWidth * mapHeight;

    for (size_t i = 0; i < mapData.size(); ++i) {
        int tileNum = mapData[i];
        if (tileNum < 0) continue; // 빈 타일(투명) 스킵

        // ★ 핵심: 인덱스가 layerSize를 넘어가면 다시 0, 0 좌표부터 겹쳐서 그리도록 만듭니다.
        int localIndex = i % layerSize;

        float tu = static_cast<float>((tileNum % sheetColumns) * tileSize);
        float tv = static_cast<float>((tileNum / sheetColumns) * tileSize);

        // i가 아닌 localIndex를 사용하여 좌표 계산
        float x = static_cast<float>(localIndex % mapWidth) * (tileSize * scale);
        float y = static_cast<float>(localIndex / mapWidth) * (tileSize * scale);
        float size = tileSize * scale;

        // 삼각형 1
        addVertex(x, y, tu, tv);
        addVertex(x + size, y, tu + tileSize, tv);
        addVertex(x, y + size, tu, tv + tileSize);

        // 삼각형 2
        addVertex(x, y + size, tu, tv + tileSize);
        addVertex(x + size, y, tu + tileSize, tv);
        addVertex(x + size, y + size, tu + tileSize, tv + tileSize);
    }

    return true;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    // ★ 변경됨: 포인터이므로 주소 연산자(&)를 뺍니다!
    states.texture = m_tileset;
    target.draw(m_vertices, states);
}