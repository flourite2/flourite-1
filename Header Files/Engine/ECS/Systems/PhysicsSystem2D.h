// PhysicsSystem2D.h

#pragma once
#include "Engine/ECS/Entities/EntityManager.h"


class PhysicsSystem2D {
private:
	// 공간 분할 격자 (Spatial Partitioning)
    std::vector<std::vector<Entity>> grid;
    int gridWidth;
    int gridHeight ;
    
    float mapW = 2000.0f;
    float mapH = 2000.0f;

    // 순수 수학 및 기하학 처리 함수 (RigidBody 대신 순수 데이터만 받음)
    bool CheckCircleVsCircle(const Collider& c1, const Vector2D& p1, const Collider& c2, const Vector2D& p2, Vector2D& normal, float& overlap);
    bool CheckCircleVsAABB(const Collider& c1, const Vector2D& p1, const Collider& c2, const Vector2D& p2, Vector2D& normal, float& overlap);
    bool CheckAABBVsAABB(const Collider& c1, const Vector2D& p1, const Collider& c2, const Vector2D& p2, Vector2D& normal, float& overlap);

    // 컴포넌트 조작 함수
    void ApplyPhysics(PhysicsComponent& phys, TransformComponent& trans, float dt);
    void ResolveCollision(TransformComponent& t1, PhysicsComponent& p1, TransformComponent& t2, PhysicsComponent& p2);
    void ApplyResolution(TransformComponent& t1, PhysicsComponent& p1, TransformComponent& t2, PhysicsComponent& p2, const Vector2D& normal, float overlap);

public:
    // 렌더 시스템으로부터 카메라 정보를 받아오기 위한 구조체
    struct CullRect {
        float left, right, top, bottom;
    };

    void SetMapBounds(float w, float h) { mapW = w; mapH = h; }

    PhysicsSystem2D(int width = 40, int height = 40);

    // 시스템의 유일한 진입점 (이제 EntityManager를 외부에서 주입받습니다)
    int FixedUpdate(EntityManager& registry, float dt, const CullRect& cullRect);
};