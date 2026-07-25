// Collider.h

#pragma once
#include "Engine/Core/Math/Vector2D.h"

enum class ColliderType {
    CIRCLE,
    AABB
};

struct Collider {
    ColliderType type = ColliderType::CIRCLE;
    Vector2D offset;

    // 원형 충돌체 데이터
    float radius = 0.0f;
    float radiusSq = 0.0f;

    // 사각형(AABB) 충돌체 데이터
    float halfWidth = 0.0f;
    float halfHeight = 0.0f;

    static Collider CreateCircleCollider(float r, Vector2D off = { 0, 0 }) {
        Collider c;
        c.type = ColliderType::CIRCLE;
        c.offset = off;
        c.radius = r;
        c.radiusSq = r * r;
        return c;
    }

    static Collider CreateAABBCollider(float w, float h, Vector2D off = { 0,0 }) {
        Collider c;
        c.type = ColliderType::AABB;
        c.offset = off;
        c.halfWidth = w / 2.0f;
        c.halfHeight = h / 2.0f;
        return c;
    }
};