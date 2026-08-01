// FSMSystem.h
#pragma once
#include "Engine/ECS/Entities/EntityManager.h"

class FSMSystem {
public:
    void Update(EntityManager& registry, float dt, float targetVel);
    static Vector2D GetRandomPatrolPoint(const Vector2D& currentPos, float radius); // static + public
};