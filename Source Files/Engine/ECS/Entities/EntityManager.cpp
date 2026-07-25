// EntityManager.cpp

#include "Engine/ECS/Entities/EntityManager.h"

EntityManager::EntityManager() {
    // 메모리 풀 미리 할당 (Allocations 스파이크 방지)
    signatures.resize(MAX_ENTITIES);
    transforms.resize(MAX_ENTITIES);
    physics.resize(MAX_ENTITIES);
    renders.resize(MAX_ENTITIES);

    fsms.resize(MAX_ENTITIES); // ★ 풀 메모리 할당
}

Entity EntityManager::CreateEntity() {
    Entity id;

    if (!availableIDs.empty()) {
        id = availableIDs.front();
        availableIDs.pop();
    }
    else {
        id = entityCounter++;
    }
    signatures[id].reset();
    activeEntityCount++;
    return id;
}

void EntityManager::DestroyEntity(Entity e) {
    signatures[e].reset(); // 비트마스크를 꺼서 모든 시스템에서 무시되게 만듦
    availableIDs.push(e);  // 큐에 반납하여 다음 Create 때 재활용
    activeEntityCount--;
}

void EntityManager::AddTransform(Entity e, const TransformComponent& comp) {
    transforms[e] = comp;
    signatures[e].set(COMP_TRANSFORM); // "이 엔티티는 Transform을 가짐" 표시
}

void EntityManager::AddPhysics(Entity e, const PhysicsComponent& comp) {
    physics[e] = comp;
    signatures[e].set(COMP_PHYSICS);
}

void EntityManager::AddRender(Entity e, const RenderComponent& comp) {
    renders[e] = comp;
    signatures[e].set(COMP_RENDER); // 렌더 컴포넌트 비트마스크 ON
}

// 장착 함수 구현
void EntityManager::AddFSM(Entity e, const FSMComponent& comp) {
    fsms[e] = comp;
    signatures[e].set(COMP_FSM); // 비트마스크 ON
}