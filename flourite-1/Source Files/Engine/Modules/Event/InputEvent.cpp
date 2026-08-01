// InputEvent.cpp

#include "Engine/Modules/Event/InputEvent.h"
#include <cmath>

namespace Event {
    bool InteractionByPlayer(const TransformComponent& playerTrans, const PhysicsComponent& playerPhys,
        const TransformComponent& otherTrans, const PhysicsComponent& otherPhys,
        float interactionRange) {

        Vector2D pos1 = playerTrans.position;
        Vector2D pos2 = otherTrans.position;
        Vector2D delta = pos2 - pos1;
        float distSq = delta.LengthSquared();

        Vector2D faced = playerTrans.facedDirection;

        // [안전 장치] 두 객체가 완전히 같은 위치(delta == 0)일 때 Normalize()로 인한 NaN 오류 방지
        if (delta.IsZero()) return false;

        float dot = faced.Dot(delta.Normalize());
        float threshold = playerPhys.collider.radius + otherPhys.collider.radius + interactionRange;

        return distSq < (threshold * threshold) && dot >= 0.707f;
    }
}