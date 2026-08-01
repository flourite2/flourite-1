// SpawnSystem.cpp

#include "Engine/ECS/Systems/SpawnsSystem.h"
#include "Engine/Core/Geometry/Collider.h"
#include <iostream>

// -----------------------------------------------------------------------
// 렌더 레이어 상수 (RenderSystem의 정렬 기준과 맞춰야 합니다)
// -----------------------------------------------------------------------
static constexpr int LAYER_GROUND = 0;   // 바닥, 정적 소품
static constexpr int LAYER_ENTITY = 1;   // 플레이어, NPC (Y-Sort 대상)
static constexpr int LAYER_OVERHEAD = 2;   // 나무 꼭대기, 가로등 상단 등

// -----------------------------------------------------------------------
// 내부 헬퍼
// -----------------------------------------------------------------------
AIType SpawnSystem::ParseAIType(const std::string& str) {
    if (str == "STATIONARY") return AIType::STATIONARY;
    if (str == "WAYPOINT")   return AIType::WAYPOINT;
    if (str == "CHATTERBOX") return AIType::CHATTERBOX;
    return AIType::NONE;
}

// TransformComponent + PhysicsComponent 공통 조립
static void AssembleCommon(Entity e, const BeingMapData& data,
    const SpawnPointData& sp, EntityManager& registry)
{
    TransformComponent tr;
    tr.position = Vector2D(sp.posX, sp.posY);
    tr.previousPosition = tr.position;
    tr.scale = Vector2D(1.0f, 1.0f);
    tr.facedDirection = Vector2D(1.0f, 0.0f); // 기본: 오른쪽
    registry.AddTransform(e, tr);

    PhysicsComponent ph;
    ph.mass = data.mass;
    ph.hasCollider = (data.radius > 0.0f);
    if (ph.hasCollider) {
        ph.collider = Collider::CreateCircleCollider(data.radius);
    }
    // mass == 0 인 정적 소품은 충돌만 있고 이동은 없음
    ph.isPushable = (data.mass > 0.0f);
    registry.AddPhysics(e, ph);
}

// -----------------------------------------------------------------------
// SpawnAll: 스폰 포인트 × 청사진 테이블 매핑 후 조립 위임
// -----------------------------------------------------------------------
void SpawnSystem::SpawnAll(
    const std::unordered_map<std::string, BeingMapData>& beingTable,
    const std::vector<SpawnPointData>& spawnPoints,
    EntityManager& registry,
    ITextureProvider& textures)
{
    for (const SpawnPointData& sp : spawnPoints) {
        // 스폰 포인트 이름으로 청사진을 검색
        auto it = beingTable.find(sp.entityID);
        if (it == beingTable.end()) {
            std::cerr << "[SpawnSystem] 청사진 없음: '" << sp.entityID << "' - 건너뜀\n";
            continue;
        }

        const BeingMapData& data = it->second;
        Entity e = registry.CreateEntity();

        // 공통 물리/트랜스폼 먼저 조립
        AssembleCommon(e, data, sp, registry);

        // Type 문자열로 전문 조립 함수 위임
        const std::string& t = data.type;

        if (t == "Player" || t == "player")  AssemblePlayer(e, data, registry, textures);
        else if (t == "NPC")          AssembleNPC(e, data, registry, textures);
        else if (t == "Prop_Dynamic") AssemblePropDynamic(e, data, registry, textures);
        else if (t == "Prop_Static")  AssemblePropStatic(e, data, registry, textures);
        else if (t == "Prop_Overhead") AssemblePropOverhead(e, data, registry, textures);
        else if (t == "Dummy")        AssembleDummy(e, data, registry, textures);
        else {
            std::cerr << "[SpawnSystem] 알 수 없는 Type: '" << t << "'\n";
        }

        std::cout << "[SpawnSystem] 소환 완료: id=" << data.id
            << " type=" << t
            << " at (" << sp.posX << ", " << sp.posY << ")\n";
    }
}

// -----------------------------------------------------------------------
// Player
// -----------------------------------------------------------------------
void SpawnSystem::AssemblePlayer(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_ENTITY;
    registry.AddRender(e, rc);

    registry.signatures[e].set(COMP_PLAYER_TAG);

    // 플레이어는 AI 없음: FSMComponent 미장착
}

// -----------------------------------------------------------------------
// NPC: AIType에 따라 FSMComponent 초기 상태를 다르게 설정
// -----------------------------------------------------------------------
void SpawnSystem::AssembleNPC(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_ENTITY;
    registry.AddRender(e, rc);

    registry.signatures[e].set(COMP_NPC_TAG);

    // AIType 파싱
    AIType ai = ParseAIType(data.aiType);

    FSMComponent fsm;
    fsm.aiType = ai;

    switch (ai) {

        // 가만히 서 있는 NPC: 대화 요청이 올 때만 CONVERSATION 으로 전환
    case AIType::STATIONARY:
        fsm.currentState = FSMState::IDLE;
        // stateTimer 를 매우 큰 값으로 초기화 → FSMSystem의 3초 타이머가
        // 절대 만료되지 않아 PATROL로 넘어가지 않음
        fsm.stateTimer = 1e9f;
        break;

        // 웨이포인트 순찰 NPC: 웨이포인트는 별도 데이터로 채워야 합니다.
        // 지금은 시작 위치를 중심으로 랜덤 패트롤로 폴백합니다.
    case AIType::WAYPOINT:
        fsm.currentState = FSMState::PATROL;
        fsm.stateTimer = 0.0f;
        // 웨이포인트가 비어 있으면 FSMSystem이 랜덤 패트롤로 처리합니다.
        break;

        // 먼저 말을 거는 NPC: 플레이어가 반경 내 진입 시 CONVERSATION 으로 전환하는
        // 로직은 InteractionSystem에서 처리합니다. 여기서는 IDLE로 대기.
    case AIType::CHATTERBOX:
        fsm.currentState = FSMState::IDLE;
        fsm.stateTimer = 0.0f;
        break;

        // AIType::NONE 또는 미래 확장 타입
    default:
        fsm.currentState = FSMState::IDLE;
        fsm.stateTimer = 0.0f;
        break;
    }

    registry.AddFSM(e, fsm);
}

// -----------------------------------------------------------------------
// Prop_Dynamic: 쓰레기통 등 밀 수 있는 오브젝트
// -----------------------------------------------------------------------
void SpawnSystem::AssemblePropDynamic(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_ENTITY; // Y-Sort 대상
    registry.AddRender(e, rc);

    // 물리는 AssembleCommon에서 이미 세팅됨 (isPushable = true)
}

// -----------------------------------------------------------------------
// Prop_Static: 나무 하단, 가로등 하단 등
// -----------------------------------------------------------------------
void SpawnSystem::AssemblePropStatic(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_ENTITY; // Y-Sort 대상 (발 위치 기준 정렬)
    registry.AddRender(e, rc);

    // mass == 0 → isPushable = false (AssembleCommon에서 자동 처리됨)
}

// -----------------------------------------------------------------------
// Prop_Overhead: 나무 꼭대기, 가로등 상단 등 (항상 위에 그려짐)
// -----------------------------------------------------------------------
void SpawnSystem::AssemblePropOverhead(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_OVERHEAD; // Y-Sort 무관, 항상 최상위
    registry.AddRender(e, rc);
}

// -----------------------------------------------------------------------
// Dummy: 테스트용, FSM + 렌더 모두 붙임
// -----------------------------------------------------------------------
void SpawnSystem::AssembleDummy(Entity e, const BeingMapData& data,
    EntityManager& registry, ITextureProvider& textures)
{
    RenderComponent rc;
    rc.texture = textures.GetTexture(data.textureKey);
    rc.animKey = data.textureKey;
    rc.layer = LAYER_ENTITY;
    registry.AddRender(e, rc);

    FSMComponent fsm;
    fsm.aiType = AIType::WAYPOINT; // 기본으로 돌아다님
    fsm.currentState = FSMState::IDLE;
    registry.AddFSM(e, fsm);
}