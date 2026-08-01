// AnimationHelper.h

#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "Engine/Modules/Render/RenderParser.h" // RenderMapData 구조체 필요
#include "Engine/Modules/Render/MotionTypes.h"  // Direction, AnimFrame 열거형 필요

namespace AnimationHelper {
    static sf::IntRect GetRectFromTileID(int tileID, int tileSize = 16, int sheetColumns = 27) {
        int x = (tileID % sheetColumns) * tileSize;
        int y = (tileID / sheetColumns) * tileSize;

        // ★ SFML 3.x 스타일로 수정: {{x, y}, {width, height}}
        return sf::IntRect({ x, y }, { tileSize, tileSize });
    }
}