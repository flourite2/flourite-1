// FSMComponent.h

#pragma once
#include "Engine/Core/Math/Vector2D.h"
#include <cstdint> // uint32_t 사용을 위해 필수
#include <vector>

enum class FSMState {
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    CONVERSATION
};

// ★ 열거형 추가 (FSMComponent 위쪽에 작성)
enum class AIType {
    NONE,
    STATIONARY,
    WAYPOINT,
    CHATTERBOX
};

struct FSMComponent {
    FSMState currentState = FSMState::IDLE;
    AIType aiType = AIType::STATIONARY;
    float stateTimer = 0.0f;

    // ★ 복구됨: 기존 FSMSystem에서 쓰던 목적지 변수
    Vector2D targetPos = Vector2D(0.0f, 0.0f);

    // 막힘 방지(Stuck Detection) 용도
    Vector2D lastPosition = Vector2D(0.0f, 0.0f);
    float stuckTimer = 0.0f;

    // 경로 및 추적 용도
    std::vector<Vector2D> waypoints;
    int currentWaypointIndex = 0;

    // ★ 순환 참조 방지: Entity 대신 원시 타입(uint32_t) 사용
    uint32_t targetNPC = 0;

    int dialogID = -1; // ★ 추가: 이 NPC가 가진 대화 시작 ID
};