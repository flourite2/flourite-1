// Engine/Modules/Render/AnimationHelper.cpp

#include "Engine/Modules/Render/AnimationHelper.h"

namespace AnimationHelper {
    sf::IntRect GetRectFromTileID(int tileID, SpriteSheetSpec sps) {
        int x = (tileID % sps.sheetColumns) * sps.tileSize;
        int y = (tileID / sps.sheetColumns) * sps.tileSize;

        // ★ SFML 3.x 스타일로 수정: {{x, y}, {width, height}}
        return sf::IntRect({ x, y }, { sps.tileSize, sps.tileSize });
    }
}