// AnimationUpdater.h

#pragma once

#include "Engine/Modules/Render/MotionTypes.h"
#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/Modules/Render/AnimationHelper.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace AnimationUpdater {

    // ECS 레지스트리와 특정 엔티티, 델타 타임, 애니메이션 테이블을 받아 렌더링 스프라이트를 갱신합니다.
    void UpdateAnimation(EntityManager& registry, Entity e, float dt, const std::unordered_map<std::string, std::vector<std::vector<int>>>& animTables, const AnimationHelper::SpriteSheetSpec& sps);
}