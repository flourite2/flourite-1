// AnimationHelper.h

#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "Engine/Modules/Render/RenderParser.h" // RenderMapData 구조체 필요
#include "Engine/Modules/Render/MotionTypes.h"  // Direction, AnimFrame 열거형 필요

namespace AnimationHelper {
    extern std::unordered_map<std::string, sf::Texture> textureCache;
    // Direction과 AnimFrame의 Count를 4와 3으로 가정 (구현에 맞게 수정 가능)
    extern const sf::Texture* animLookupTable[4][3];
    extern std::string keyLookupTable[4][3];

    void Initialize(const std::unordered_map<std::string, RenderMapData>& renderTable, const std::string& basePath);

    static sf::IntRect GetRectFromTileID(int tileID, int tileSize = 16, int sheetColumns = 27) {
        int x = (tileID % sheetColumns) * tileSize;
        int y = (tileID / sheetColumns) * tileSize;

        // ★ SFML 3.x 스타일로 수정: {{x, y}, {width, height}}
        return sf::IntRect({ x, y }, { tileSize, tileSize });
    }
}