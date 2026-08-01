// InputEvent.h

#pragma once
#include "Engine/ECS/Components/Physics/TransformComponent.h"
#include "Engine/ECS/Components/Physics/PhysicsComponent.h"

namespace Event {
    // 더 이상 RigidBody가 아닌, ECS의 순수 데이터들을 인자로 받습니다.
    bool InteractionByPlayer(const TransformComponent& playerTrans, const PhysicsComponent& playerPhys,
        const TransformComponent& otherTrans, const PhysicsComponent& otherPhys,
        float interactionRange);
}