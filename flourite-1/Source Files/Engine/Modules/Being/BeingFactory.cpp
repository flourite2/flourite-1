// BeingFactory.cpp

#include "Engine/Modules/Being/BeingFactory.h"

Entity BeingFactory::SpawnBeing(EntityManager& registry, const BeingMapData& data, const sf::Texture* atlas, int initialTileID, SpawnDefault spawnDefaults, const AnimationHelper::SpriteSheetSpec sps, float x, float y) {
    Entity e = registry.CreateEntity();
    
    // 생성 검증
    if (e == INVALID_ENTITY) return e;

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
    rend.textureRect = AnimationHelper::GetRectFromTileID(initialTileID, sps);
    rend.origin = sf::Vector2f(8.f, 8.f);
    rend.color = sf::Color::White;
    rend.animKey = data.textureKey;
    rend.layer = (data.type == "Prop_Overhead") ? 2 : 1;

    registry.AddRender(e, rend);
    return e;
}