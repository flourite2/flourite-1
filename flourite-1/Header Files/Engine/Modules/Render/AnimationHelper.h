// Engine/Modules/Render/AnimationHelper.h

#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>    

namespace AnimationHelper {
    struct SpriteSheetSpec {
        int tileSize;
        int sheetColumns;
    };

    sf::IntRect GetRectFromTileID(int tileID, SpriteSheetSpec sps);
}