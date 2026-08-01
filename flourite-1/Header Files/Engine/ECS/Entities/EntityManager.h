#pragma once
#include <vector>
#include <bitset>
#include <cstdint>
#include <queue>
#include "Engine/ECS/Components/Physics/TransformComponent.h"
#include "Engine/ECS/Components/Physics/PhysicsComponent.h"
#include "Engine/ECS/Components/Render/RenderComponent.h"
#include "Engine/ECS/Components/AI/FSMComponent.h" // ★ 신규 추가

using Entity = uint32_t;
const int MAX_ENTITIES = 5000;

// 1. 컴포넌트 타입 열거형
enum ComponentType {
    COMP_TRANSFORM = 0,
    COMP_PHYSICS = 1,
    COMP_RENDER = 2,
    COMP_FSM = 3,

    // ★ 신규 추가
    COMP_PLAYER_TAG = 4,
    COMP_NPC_TAG = 5
};

// 2. 태그용 빈 구조체 (enum 바로 아래에 추가)
struct PlayerTag {};
struct NPCTag {};

class EntityManager {
private:
    Entity entityCounter = 0;
    std::queue<Entity> availableIDs; // 반납된 ID를 저장할 큐
    uint32_t activeEntityCount = 0;  // 현재 살아있는 엔티티 수

public:
    EntityManager();

    // 비트마스크: 각 엔티티가 어떤 컴포넌트를 들고 있는지 0과 1로 체크
    std::vector<std::bitset<32>> signatures;

    Entity CreateEntity();
    void DestroyEntity(Entity e);    // 새로 추가
    uint32_t GetEntityCapacity() const { return entityCounter; } // 9번 결함을 위한 추가

    
    // 컴포넌트 풀 (데이터가 메모리에 일렬로 나열됨)
    std::vector<TransformComponent> transforms;
    std::vector<PhysicsComponent> physics;
    std::vector<RenderComponent> renders; // RenderComponent 정의 후 주석 해제

    // 컴포넌트 장착 함수들
    void AddTransform(Entity e, const TransformComponent& comp);
    void AddPhysics(Entity e, const PhysicsComponent& comp);
    void AddRender(Entity e, const RenderComponent& comp);

    // 2. FSM 컴포넌트 풀 생성
    std::vector<FSMComponent> fsms;

    // 3. 컴포넌트 장착 함수 선언
    void AddFSM(Entity e, const FSMComponent& comp);
};