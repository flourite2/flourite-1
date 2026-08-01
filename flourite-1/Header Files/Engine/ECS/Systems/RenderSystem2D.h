// RenderSystem2D.h

#pragma once
#include <vector>
#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/Modules/Render/Renderer.h" 

class RenderSystem2D {
private:
    Renderer& renderer;
    std::vector<Entity> renderables;
    uint32_t maxIndex = 0;

public:
    // 렌더러 참조를 주입받아 생성됨
    RenderSystem2D(Renderer& r);

    // GameObject 배열 대신 EntityManager를 받습니다.
    // alpha 값은 TimeSystem의 GetAlpha()를 받아 렌더링 보간을 할 때 씁니다.
    void Render(EntityManager& registry, float alpha = 0.0f);
};