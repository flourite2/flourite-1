// AnimationUpdater.h

#pragma once

#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/Modules/Render/AnimationHelper.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath> // std::abs 사용을 위해 추가

namespace AnimationUpdater {

    // ECS 레지스트리와 특정 엔티티, 델타 타임, 애니메이션 테이블을 받아 렌더링 스프라이트를 갱신합니다.
    inline void UpdateAnimation(EntityManager& registry, Entity e, float dt, const std::unordered_map<std::string, std::vector<std::vector<int>>>& animTables) {
        auto& phys = registry.physics[e];
        auto& trans = registry.transforms[e];
        auto& rend = registry.renders[e];

        // 움직이지 않는 프롭(Prop)이거나, 애니메이션 데이터가 없는 경우 연산 조기 종료
        if (rend.animKey.empty() || animTables.find(rend.animKey) == animTables.end()) return;

        const auto& animTable = animTables.at(rend.animKey);

        // 일정 속도 이상 & 실제 위치 변화가 있을 때만 이동 중인 것으로 간주
        bool isMoving = (phys.velocity.LengthSquared() > 50.0f) &&
            (trans.position - trans.previousPosition).LengthSquared() > 0.5f;

        Direction currentDir = Direction::Down;
        AnimFrame currentFrame = AnimFrame::Stand;

        // 바라보는 방향 계산 (X축 이동이 크면 좌우, Y축 이동이 크면 상하)
        if (std::abs(trans.facedDirection.x) > std::abs(trans.facedDirection.y)) {
            currentDir = (trans.facedDirection.x < 0) ? Direction::Left : Direction::Right;
        }
        else {
            currentDir = (trans.facedDirection.y < 0) ? Direction::Up : Direction::Down;
        }

        // 애니메이션 타이머 및 프레임 틱 진행
        if (isMoving) {
            rend.animTimer += dt;
            if (rend.animTimer >= 0.15f) {
                rend.animTimer = 0.f;
                rend.animTick++;
            }
            int frameCycle = rend.animTick % 4;
            if (frameCycle == 0) currentFrame = AnimFrame::Walk1;
            else if (frameCycle == 2) currentFrame = AnimFrame::Walk2;
            else currentFrame = AnimFrame::Stand;
        }
        else {
            currentFrame = AnimFrame::Stand;
            rend.animTimer = 0.f;
        }

        // 방향을 테이블의 열(Column) 인덱스로 변환
        int colIndex = 1;
        switch (currentDir) {
        case Direction::Left:  colIndex = 0; break;
        case Direction::Down:  colIndex = 1; break;
        case Direction::Up:    colIndex = 2; break;
        case Direction::Right: colIndex = 3; break;
        }

        // 프레임 상태를 행(Row) 인덱스로 변환하여 스프라이트 사각형 갱신
        int rowIndex = static_cast<int>(currentFrame);
        rend.textureRect = AnimationHelper::GetRectFromTileID(animTable[rowIndex][colIndex]);
    }

} // namespace AnimationUpdater