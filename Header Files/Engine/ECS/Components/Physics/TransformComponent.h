// TransformComponent.h

#pragma once
#include "Engine/Core/Math/Vector2D.h"

struct TransformComponent {
    Vector2D position;
    Vector2D previousPosition; // 물리 충돌 복원 및 렌더링 보간(Alpha)용
    Vector2D scale = { 1.0f, 1.0f };
    Vector2D facedDirection = { 1.0f, 0.0f }; // 캐릭터가 바라보는 방향
};