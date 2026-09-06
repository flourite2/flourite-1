// BeingFactory.h

#pragma once
#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/Modules/Being/BeingParser.h" // BeingMapData »ç¿ë
#include "Engine/Modules/Render/AnimationHelper.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace BeingFactory {
    struct SpawnDefault {
        float damping;  
    };

    Entity SpawnBeing(EntityManager& registry, const BeingMapData& data, const sf::Texture* atlas, int initialTileID, SpawnDefault spawnDefaults, const AnimationHelper::SpriteSheetSpec sps, float x, float y);
}
