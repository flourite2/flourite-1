// PhysicsComponent.h

#pragma once
#include "Engine/Core/Math/Vector2D.h"
#include "Engine/Core/Geometry/Collider.h"

struct PhysicsComponent {
    // 이동 역학 데이터
    Vector2D velocity;
    Vector2D acceleration;
    Vector2D force;
    Vector2D impulse;

    // 충돌체 및 물리 속성
    Collider collider;
    float mass = 1.0f;
    float damping = 8.0f;

    // 상태 플래그
    bool isSleeping = false;
    bool hasCollider = true;
    bool isPushable = false;
};