// PhysicsSystem2D.cpp

#include <cmath>
#include <algorithm>
#include <vector>
#include "Engine/ECS/Systems/PhysicsSystem2D.h"

PhysicsSystem2D::PhysicsSystem2D(int width, int height) : gridWidth(width), gridHeight(height) {
    grid.resize(gridWidth * gridHeight);
    for (auto& cell : grid) {
        cell.reserve(50); // 한 칸에 대략 50개의 엔티티가 들어올 것을 대비해 메모리 미리 확보
    }
}

// ==============================================================================
// 1. 메인 루프 (FixedUpdate)
// ==============================================================================
int PhysicsSystem2D::FixedUpdate(EntityManager& registry, float dt, const CullRect& cullRect) {
    int activateCount = 0;
    uint32_t maxEntityIndex = registry.GetEntityCapacity();

    // 1. 활성 엔티티 물리 이동
    for (Entity i = 0; i < maxEntityIndex; ++i) {
        if (!registry.signatures[i].test(COMP_TRANSFORM) ||
            !registry.signatures[i].test(COMP_PHYSICS)) continue;

        auto& trans = registry.transforms[i];

        // ★ 컬링: 범위 밖이면 sleep 처리 후 스킵
        if (trans.position.x < cullRect.left || trans.position.x > cullRect.right ||
            trans.position.y < cullRect.top || trans.position.y > cullRect.bottom) {
            registry.physics[i].isSleeping = true;
            continue;
        }

        // [수정할 코드] 속도(Velocity)가 있어도 깨우도록 수정
        auto& phys = registry.physics[i];
        if (!phys.force.IsZero() || !phys.impulse.IsZero() || !phys.velocity.IsZero()) phys.isSleeping = false;
        if (!phys.isSleeping) {
            ApplyPhysics(phys, trans, dt);
            activateCount++;
        }
    }

    // ==============================================================================
    // 2. 공간 분할 격자 (Spatial Partitioning) - Broad Phase
    // ==============================================================================
    const float CELL_SIZE = 120.0f;

    // ★ 수정 1: 매 프레임 vector를 새로 만들지 않고, 생성자에서 reserve 해둔 멤버 변수 grid를 비워서 재사용합니다.
    for (auto& cell : grid) {
        cell.clear();
    }

    // [2-1] 모든 물리 엔티티를 격자에 배치 (Mapping)
    for (Entity i = 0; i < maxEntityIndex; ++i) {
        if (!registry.signatures[i].test(COMP_TRANSFORM) || !registry.signatures[i].test(COMP_PHYSICS)) continue;

        auto& trans = registry.transforms[i];
        auto& phys = registry.physics[i];

        // ★ 수정 2 & 3: GRID_WIDTH 대문자 제거 및 clamp 타임 캐스팅
        // 타입 추론 에러를 완벽히 막기 위해 최대값을 명시적 int로 변환해 둡니다.
        int maxW = static_cast<int>(gridWidth) - 1;
        int maxH = static_cast<int>(gridHeight) - 1;

        if (phys.collider.type == ColliderType::AABB && phys.mass <= 0.0) {

            int minCol = std::clamp<int>(static_cast<int>((trans.position.x - phys.collider.halfWidth) / CELL_SIZE), 0, maxW);
            int maxCol = std::clamp<int>(static_cast<int>((trans.position.x + phys.collider.halfWidth) / CELL_SIZE), 0, maxW);
            int minRow = std::clamp<int>(static_cast<int>((trans.position.y - phys.collider.halfHeight) / CELL_SIZE), 0, maxH);
            int maxRow = std::clamp<int>(static_cast<int>((trans.position.y + phys.collider.halfHeight) / CELL_SIZE), 0, maxH);

            for (int r = minRow; r <= maxRow; ++r) {
                for (int c = minCol; c <= maxCol; ++c) {
                    grid[r * gridWidth + c].push_back(i);
                }
            }
        }
        else {
            int col = std::clamp<int>(static_cast<int>(trans.position.x / CELL_SIZE), 0, maxW);
            int row = std::clamp<int>(static_cast<int>(trans.position.y / CELL_SIZE), 0, maxH);

            grid[row * gridWidth + col].push_back(i);
        }
    }

    // ==============================================================================
    // 3. 충돌 해소 - Narrow Phase (격자 내부 순회)
    // ==============================================================================

    for (int r = 0; r < gridHeight; ++r) {
        for (int c = 0; c < gridWidth; ++c) {
            size_t cellIndex = static_cast<size_t>(r) * gridWidth + c;
            const auto& cell = grid[cellIndex];

            // 현재 칸(cell)에 모여있는 엔티티들 순회
            for (size_t i = 0; i < cell.size(); ++i) {
                Entity e1 = cell[i];
                auto& p1 = registry.physics[e1];
                auto& t1 = registry.transforms[e1];

                // [3-1] 나와 '같은 방'에 있는 엔티티들과 충돌 검사
                for (size_t j = i + 1; j < cell.size(); ++j) {
                    Entity e2 = cell[j];
                    auto& p2 = registry.physics[e2];
                    auto& t2 = registry.transforms[e2];

                    // 수정 후: 둘 중 하나라도 mass=0(벽)이면 sleeping 여부 무관하게 충돌 체크
                    if (p1.mass <= 0.0 && p2.mass <= 0.0) continue; // 벽끼리는 스킵
                    if (p1.isSleeping && p2.isSleeping && p1.mass > 0.0f && p2.mass > 0.0f) continue; // 동적 객체끼리 둘 다 sleeping일 때만 스킵
                    ResolveCollision(t1, p1, t2, p2);
                }

                // [3-2] '인접한 방'의 엔티티들과 충돌 검사 
                // 중복 연산(예: A가 B를 검사했는데 B가 A를 또 검사하는 것)을 막기 위해 8방향이 아닌 4방향만 검사합니다.
                int neighborOffsets[4][2] = { {1, 0}, {-1, 1}, {0, 1}, {1, 1} }; // 우, 좌하, 하, 우하

                for (auto& offset : neighborOffsets) {
                    int nr = r + offset[1];
                    int nc = c + offset[0];

                    if (nr >= 0 && nr < gridHeight && nc >= 0 && nc < gridWidth) {
                        size_t neighborIndex = static_cast<size_t>(nr) * gridWidth + nc;
                        const auto& neighborCell = grid[neighborIndex];

                        for (Entity e2 : neighborCell) {
                            auto& p2 = registry.physics[e2];
                            auto& t2 = registry.transforms[e2];

                            if ((p1.mass <= 0.0 && p2.mass <= 0.0) || (p1.isSleeping && p2.isSleeping)) continue;
                            ResolveCollision(t1, p1, t2, p2);
                        }
                    }
                }
            } // end of e1 loop
        }
    }

    return activateCount;
}

// ==============================================================================
// 2. 물리 적용 및 해소 (Apply & Resolve)
// ==============================================================================
void PhysicsSystem2D::ApplyPhysics(PhysicsComponent& phys, TransformComponent& trans, float dt) {
    if (phys.isSleeping) { return; }

    // ★ 복구: 가속도(지속 힘) 연산과 충격량(순간 힘) 연산 분리 적용
    if (phys.mass > 0.0) {
        phys.acceleration = phys.force / phys.mass;
        phys.velocity += phys.impulse / phys.mass; // 잃어버린 Impulse 적용
    }
    else {
        phys.acceleration = Vector2D(0, 0);
    }
    phys.velocity += phys.acceleration * dt;

    // ApplyPhysics 내부 감쇠 적용 부분 수정 제안
    if (phys.mass > 0.0) {
        // 플레이어 등 질량이 있는 동적 객체는 관성과 마찰력(감쇠) 적용
        phys.velocity = phys.velocity * std::exp(-phys.damping * dt);
    }

    const float stopThreshold = 0.01f;
    if (phys.velocity.Length() < stopThreshold) {
        phys.velocity = Vector2D(0, 0);
    }

    if (phys.force.LengthSquared() > 0.001) {
        trans.facedDirection = phys.force.Normalize();
    }

    trans.previousPosition = trans.position;
    trans.position += phys.velocity * dt;

    // 충격량 초기화
    phys.impulse = Vector2D(0, 0);

    if (phys.velocity.IsZero() && phys.force.IsZero() && phys.impulse.IsZero()) {
        phys.isSleeping = true;
    }

    // 맵 경계 클램프 
    trans.position.x = std::clamp(trans.position.x, 0.0f, mapW);
    trans.position.y = std::clamp(trans.position.y, 0.0f, mapH);
}

void PhysicsSystem2D::ResolveCollision(TransformComponent& t1, PhysicsComponent& p1, TransformComponent& t2, PhysicsComponent& p2) {
    Vector2D normal;
    float overlap = 0.0;;
    bool collided = false;

    auto type1 = p1.collider.type;
    auto type2 = p2.collider.type;

    if (type1 == ColliderType::CIRCLE && type2 == ColliderType::CIRCLE) {
        collided = CheckCircleVsCircle(p1.collider, t1.position, p2.collider, t2.position, normal, overlap);
    }
    else if (type1 == ColliderType::AABB && type2 == ColliderType::AABB) {
        collided = CheckAABBVsAABB(p1.collider, t1.position, p2.collider, t2.position, normal, overlap);
    }
    else {
        if (type1 == ColliderType::CIRCLE) {
            collided = CheckCircleVsAABB(p1.collider, t1.position, p2.collider, t2.position, normal, overlap);
        }
        else {
            collided = CheckCircleVsAABB(p2.collider, t2.position, p1.collider, t1.position, normal, overlap);
            normal = normal * -1.0;
        }
    }

    if (collided) {
        ApplyResolution(t1, p1, t2, p2, normal, overlap);
    }
}

void PhysicsSystem2D::ApplyResolution(TransformComponent& t1, PhysicsComponent& p1, TransformComponent& t2, PhysicsComponent& p2, const Vector2D& normal, float overlap) {
    const float slop = 0.01f;
    float actualOverlap = std::max(overlap - slop, 0.0f);

    float ratio1 = 0.0f;
    float ratio2 = 0.0f;

    // 1. 완벽한 정적 객체 (건물, 맵 경계 등 질량 0)
    if (p1.mass <= 0.0f || p2.mass <= 0.0f) {
        float invMass1 = (p1.mass <= 0.0f) ? 0.0f : 1.0f / p1.mass;
        float invMass2 = (p2.mass <= 0.0f) ? 0.0f : 1.0f / p2.mass;
        float totalInvMass = invMass1 + invMass2;
        if (totalInvMass > 0.0f) {
            ratio1 = invMass1 / totalInvMass;
            ratio2 = invMass2 / totalInvMass;
        }
    }
    // 2. 한 쪽만 밀릴 수 있는 객체 (예: Player vs Dummy)
    else if (!p1.isPushable && p2.isPushable) {
        ratio1 = 0.0f; ratio2 = 1.0f; // p2(Dummy)가 100% 밀려남
    }
    else if (p1.isPushable && !p2.isPushable) {
        ratio1 = 1.0f; ratio2 = 0.0f; // p1(Dummy)이 100% 밀려남
    }
    // 3. 둘 다 밀릴 수 있는 객체 (예: Dummy vs Dummy) -> 질량 비례로 서로 밀어냄
    else if (p1.isPushable && p2.isPushable) {
        float invMass1 = 1.0f / p1.mass;
        float invMass2 = 1.0f / p2.mass;
        ratio1 = invMass1 / (invMass1 + invMass2);
        ratio2 = invMass2 / (invMass1 + invMass2);
    }
    // 4. 둘 다 밀리지 않는 객체 (예: Player vs NPC) -> 기존의 속도 비례 로직 (서로 벽처럼 작용)
    else {
        float speed1 = p1.velocity.Length();
        float speed2 = p2.velocity.Length();
        if (speed1 < 0.01f && speed2 < 0.01f) {
            ratio1 = 0.5f; ratio2 = 0.5f;
        }
        else {
            float totalSpeed = speed1 + speed2;
            ratio1 = speed1 / totalSpeed;
            ratio2 = speed2 / totalSpeed;
        }
    }

    // 위치 보정 실행 (actualOverlap과 normal도 내부적으로 float이어야 합니다)
    t1.position -= normal * actualOverlap * ratio1;
    t2.position += normal * actualOverlap * ratio2;

    // 속도 보정 (슬라이딩) - 기존 로직 유지
    auto AdjustVelocity = [&](PhysicsComponent& phys, const Vector2D& n) {
        if (phys.mass <= 0) return;
        Vector2D v = phys.velocity;
        float vDotN = v.Dot(n);
        if (vDotN > 0) {
            phys.velocity = v - n * vDotN;
        }
        };

    AdjustVelocity(p1, normal * -1.0f);
    AdjustVelocity(p2, normal);

    p1.isSleeping = false;
    p2.isSleeping = false;

    // 정적 객체(mass=0)가 아닌데 velocity가 0이면 살짝 흔들어줌
    if (p1.mass > 0.0f && p1.velocity.IsZero()) {
        p1.velocity = normal * -0.1f;
    }
    if (p2.mass > 0.0f && p2.velocity.IsZero()) {
        p2.velocity = normal * 0.1f;
    }
}

// ==============================================================================
// 3. 기하학 충돌 판별 로직 (순수 수학 영역)
// ==============================================================================
// 기존 로직과 100% 동일합니다. 파라미터만 (Collider, Position)으로 변경되었습니다.

bool PhysicsSystem2D::CheckCircleVsCircle(const Collider& c1, const Vector2D& p1, const Collider& c2, const Vector2D& p2, Vector2D& normal, float& overlap) {
    Vector2D delta = p2 - p1;
    float distance = delta.Length();
    float sumR = c1.radius + c2.radius;

    if (distance >= sumR) return false;

    overlap = sumR - distance;
    if (distance < 0.0001f) {
        normal = Vector2D(1.0f, 0.0f);
    }
    else {
        normal = delta / distance;
    }
    return true;
}

bool PhysicsSystem2D::CheckAABBVsAABB(const Collider& c1, const Vector2D& p1, const Collider& c2, const Vector2D& p2, Vector2D& normal, float& overlap) {
    Vector2D delta = p2 - p1;
    float overlapX = (c1.halfWidth + c2.halfWidth) - std::abs(delta.x);
    if (overlapX <= 0.0f) return false;

    float overlapY = (c1.halfHeight + c2.halfHeight) - std::abs(delta.y);
    if (overlapY <= 0.0f) return false;

    if (overlapX < overlapY) {
        overlap = overlapX;
        normal = (delta.x > 0.0f) ? Vector2D(1.0f, 0.0f) : Vector2D(-1.0f, 0.0f);
    }
    else {
        overlap = overlapY;
        normal = (delta.y > 0.0f) ? Vector2D(0.0f, 1.0f) : Vector2D(0.0f, -1.0f);
    }
    return true;
}

bool PhysicsSystem2D::CheckCircleVsAABB(const Collider& circle, const Vector2D& cPos, const Collider& aabb, const Vector2D& aPos, Vector2D& normal, float& overlap) {
    Vector2D rel = cPos - aPos;

    Vector2D closest = rel;
    closest.x = std::clamp(closest.x, -aabb.halfWidth, aabb.halfWidth);
    closest.y = std::clamp(closest.y, -aabb.halfHeight, aabb.halfHeight);

    Vector2D distVec = rel - closest;
    float distSq = distVec.LengthSquared();
    float r = circle.radius;

    bool isInside = (rel.x == closest.x && rel.y == closest.y);
    if (!isInside && distSq >= r * r) return false;

    if (isInside) {
        float dL = rel.x - (-aabb.halfWidth);
        float dR = aabb.halfWidth - rel.x;
        float dT = aabb.halfHeight - rel.y;
        float dB = rel.y - (-aabb.halfHeight);

        if (dL < dR && dL < dT && dL < dB) {
            normal = Vector2D(1, 0);
            overlap = r + dL;
        }
        else if (dR < dT && dR < dB) {
            normal = Vector2D(-1, 0);
            overlap = r + dR;
        }
        else if (dT < dB) {
            normal = Vector2D(0, -1);
            overlap = r + dT;
        }
        else {
            normal = Vector2D(0, 1);
            overlap = r + dB;
        }
    }
    else {
        float dist = std::sqrt(distSq);
        normal = (closest - rel) / dist;
        overlap = r - dist;
    }

    return true;
}