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
        float damping = 8.0f;  
    };

    Entity SpawnBeing(EntityManager& registry, const BeingMapData& data, const sf::Texture* atlas, int initialTileID, SpawnDefault spawnDefaults, float overrideX = -1.0f, float overrideY = -1.0f) {
        Entity e = registry.CreateEntity();
        float x = (overrideX != -1.0f) ? overrideX : data.posX;
        float y = (overrideY != -1.0f) ? overrideY : data.posY;

        registry.AddTransform(e, TransformComponent{ Vector2D(x, y), Vector2D(x, y), Vector2D(6.f, 6.f) });

        PhysicsComponent phys;
        phys.mass = data.mass;
        phys.damping = spawnDefaults.damping;
        phys.collider = Collider::CreateCircleCollider(data.radius);

        if (data.type == "Prop_Dynamic" || data.type == "Dummy") { phys.mass = 1.0f; phys.isPushable = true; }
        else if (data.type == "Prop_Static") { phys.mass = 0.0f; phys.isPushable = false; }
        else { phys.isPushable = false; }

        if (data.radius > 0.0f) registry.AddPhysics(e, phys);

        RenderComponent rend;
        rend.texture = atlas;
        rend.textureRect = AnimationHelper::GetRectFromTileID(initialTileID);
        rend.origin = sf::Vector2f(8.f, 8.f);
        rend.color = sf::Color::White;
        rend.animKey = data.textureKey;
        rend.layer = (data.type == "Prop_Overhead") ? 2 : 1;

        registry.AddRender(e, rend);
        return e;
    }
}
