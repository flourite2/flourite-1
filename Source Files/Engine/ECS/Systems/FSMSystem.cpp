// FSMSystem.cpp
#include "Engine/ECS/Systems/FSMSystem.h"
#include "Engine/Modules/Dialog/DialogStatus.h" // 이거 한 줄 추가!
#include "Engine/Modules/Physics/PhysicsUtils.h"
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

// -----------------------------------------------------------------------
// 헬퍼 : 난수 패트롤 목표 생성 (4방향 직각 이동 전용)
// -----------------------------------------------------------------------
Vector2D FSMSystem::GetRandomPatrolPoint(const Vector2D& currentPos, float radius) {
    // 고정 시드(1234) 대신 매번 다른 패턴을 보이도록 진짜 난수 장치 사용
    static std::random_device rd;
    static std::mt19937 rng(rd());

    std::uniform_real_distribution<float> dist(-radius, radius); // 이동 거리
    std::uniform_int_distribution<int> axisChoice(0, 1);         // X축(0) vs Y축(1) 동전 던지기

    float moveAmount = dist(rng);

    if (axisChoice(rng) == 0) {
        // X축으로만 이동 (Y좌표 고정)
        return Vector2D(currentPos.x + moveAmount, currentPos.y);
    }
    else {
        // Y축으로만 이동 (X좌표 고정)
        return Vector2D(currentPos.x, currentPos.y + moveAmount);
    }
}

// -----------------------------------------------------------------------
// 막힘 감지 (Stuck Detection)
// NPC가 일정 시간 거의 움직이지 않는데 PATROL 중이면 새 목표 재지정
// -----------------------------------------------------------------------
static constexpr float STUCK_CHECK_INTERVAL = 1.5f;  // 몇 초마다 체크할 것인가
static constexpr float STUCK_DIST_SQ = 4.0f;  // 이 픽셀² 미만이면 막힌 것으로 판단

static void UpdateStuckDetection(FSMComponent& fsm, TransformComponent& trans,
    PhysicsComponent& phys, float dt)
{
    fsm.stuckTimer += dt;
    if (fsm.stuckTimer >= STUCK_CHECK_INTERVAL) {
        float movedSq = (trans.position - fsm.lastPosition).LengthSquared();
        if (movedSq < STUCK_DIST_SQ) {
            // 막힘 확인 → 새 목표 재지정
            std::cout << "[AI Stuck] NPC가 막혔습니다. 새 목표를 지정합니다.\n";
            fsm.targetPos = FSMSystem::GetRandomPatrolPoint(trans.position, 200.0f);
            phys.velocity = Vector2D(0, 0); // 관성 초기화
        }
        fsm.lastPosition = trans.position;
        fsm.stuckTimer = 0.0f;
    }
}

// -----------------------------------------------------------------------
// STATIONARY 업데이트
// 제자리 대기. 외부(InteractionSystem)에서 CONVERSATION으로 전환합니다.
// -----------------------------------------------------------------------
static void UpdateStationary(FSMComponent& fsm, PhysicsComponent& phys, float /*dt*/)
{
    phys.force = Vector2D(0, 0);
    phys.velocity = Vector2D(0, 0);
    // currentState 는 건드리지 않음 ? 항상 IDLE 유지
}

// -----------------------------------------------------------------------
// WAYPOINT 업데이트
// waypoints 배열이 있으면 순환 이동, 없으면 랜덤 패트롤 폴백
// -----------------------------------------------------------------------
static void UpdateWaypoint(FSMComponent& fsm, TransformComponent& trans,
    PhysicsComponent& phys, float dt, float targetVel)
{
    switch (fsm.currentState) {

    case FSMState::IDLE:
        phys.force = Vector2D(0, 0);
        fsm.stateTimer += dt;

        if (fsm.stateTimer > 2.0f) {  // STATIONARY(1e9) 와 달리 2초 후 이동 재개
            fsm.currentState = FSMState::PATROL;
            fsm.stateTimer = 0.0f;

            if (!fsm.waypoints.empty()) {
                // 웨이포인트 배열에서 다음 목표 선택
                fsm.currentWaypointIndex =
                    (fsm.currentWaypointIndex + 1) % static_cast<int>(fsm.waypoints.size());
                fsm.targetPos = fsm.waypoints[fsm.currentWaypointIndex];
            }
            else {
                // 웨이포인트 없음 → 랜덤 패트롤 폴백
                fsm.targetPos = FSMSystem::GetRandomPatrolPoint(trans.position, 150.0f);
                std::cout << "[AI Waypoint] 웨이포인트 없음, 랜덤 패트롤 목표: "
                    << fsm.targetPos.x << ", " << fsm.targetPos.y << "\n";
            }
        }
        break;

    case FSMState::PATROL:
    {
        Vector2D dir = fsm.targetPos - trans.position;
        float    distSq = dir.LengthSquared();

        if (distSq < 100.0f) {  // 목표 반경 10px 이내 → IDLE로 전환
            phys.force = Vector2D(0, 0);
            fsm.currentState = FSMState::IDLE;
            fsm.stateTimer = 0.0f;
        }
        else {
            phys.force = PhysicsUtils::CalculateForce(dir, targetVel, phys.mass, phys.damping);
            UpdateStuckDetection(fsm, trans, phys, dt);
        }
        break;
    }

    case FSMState::CONVERSATION:
        phys.force = Vector2D(0, 0);
        phys.velocity = Vector2D(0, 0);
        break;

    default:
        break;
    }
}

// -----------------------------------------------------------------------
// CHATTERBOX 업데이트
// IDLE 중 일정 시간이 지나면 가장 가까운 NPC(또는 플레이어)에게 말을 건다.
// 실제 대화 개시는 InteractionSystem에 이벤트를 보내는 방식으로 연결하세요.
// -----------------------------------------------------------------------
static void UpdateChatterbox(FSMComponent& fsm, TransformComponent& trans,
    PhysicsComponent& phys, float dt)
{
    switch (fsm.currentState) {

    case FSMState::IDLE:
        phys.force = Vector2D(0, 0);
        phys.velocity = Vector2D(0, 0);
        fsm.stateTimer += dt;

        if (fsm.stateTimer > 5.0f) {
            // 대상(targetNPC)이 지정되어 있으면 CONVERSATION으로 전환
            // 대상 지정은 InteractionSystem 또는 근접 탐색 로직이 담당합니다.
            if (fsm.targetNPC != 0) {
                fsm.currentState = FSMState::CONVERSATION;
                fsm.stateTimer = 0.0f;
                std::cout << "[AI Chatterbox] 대화 시작! 대상 엔티티: "
                    << fsm.targetNPC << "\n";
            }
            else {
                // 대상 없음 → 타이머 리셋 후 계속 대기
                fsm.stateTimer = 0.0f;
            }
        }
        break;

    case FSMState::CONVERSATION:
        phys.velocity = Vector2D(0, 0); // 말하는 동안 멈춰있기

        // ★ 전역 대화 상태가 꺼졌다면(대화가 끝났다면) 자동으로 IDLE로 복귀
        if (!g_DialogStatus.isSpeaking) {
            fsm.currentState = FSMState::IDLE;
            fsm.stateTimer = 0.0f;
        }
        break;
    }
}

// -----------------------------------------------------------------------
// FSMSystem::Update ? AIType으로 분기 후 전문 함수 위임
// -----------------------------------------------------------------------
void FSMSystem::Update(EntityManager& registry, float dt, float targetVel) {
    uint32_t maxEntityIndex = registry.GetEntityCapacity();

    for (Entity i = 0; i < maxEntityIndex; ++i) {
        if (!registry.signatures[i].test(COMP_FSM) ||
            !registry.signatures[i].test(COMP_TRANSFORM) ||
            !registry.signatures[i].test(COMP_PHYSICS))   continue;

        auto& fsm = registry.fsms[i];
        auto& trans = registry.transforms[i];
        auto& phys = registry.physics[i];

        // ★ [수정됨] 모든 AI의 공통 대화 상태 처리
        if (fsm.currentState == FSMState::CONVERSATION) {
            phys.force = Vector2D(0, 0);
            phys.velocity = Vector2D(0, 0);

            // 전역 대화 상태가 꺼졌다면(대화가 끝났다면) 자동으로 IDLE로 복귀
            if (!g_DialogStatus.isSpeaking) {
                fsm.currentState = FSMState::IDLE;
                fsm.stateTimer = 0.0f;
            }
            continue; // IDLE로 돌아가든 말든 이번 프레임 물리 이동은 스킵
        }

        // AIType별 전문 업데이트 함수로 위임
        switch (fsm.aiType) {
        case AIType::STATIONARY:
            UpdateStationary(fsm, phys, dt);
            break;
        case AIType::WAYPOINT:
            UpdateWaypoint(fsm, trans, phys, dt, targetVel);
            break;
        case AIType::CHATTERBOX:
            UpdateChatterbox(fsm, trans, phys, dt);
            break;
        case AIType::NONE:
        default:
            UpdateWaypoint(fsm, trans, phys, dt, targetVel);
            break;
        }
    }
}