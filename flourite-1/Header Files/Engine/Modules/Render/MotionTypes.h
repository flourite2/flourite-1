// Engine/Modules/Render/MotionTypes.h

#pragma once

// 애니메이션 구현을 위한 방향 매핑

// 스프라이트 파일에서 알 수 없는 정보기 때문에 이름으로 사용해야 함. 열 매핑은 AnimationUpdater::UpdateAnimation()에서 함.
enum class Direction {
    Down,
    Up,
    Left,
    Right,
    Count
};
constexpr size_t DIRECTION_COUNT = static_cast<size_t>(Direction::Count);

// 애니메이션 프레임 매핑

// 해당 정보의 순서는 스프라이트 파일에 종속되기 때문에 해당하는 값을 부여
enum class AnimFrame {
    Stand = 0,
    Walk1 = 1,
    Walk2 = 2,
    Count
};
constexpr size_t ANIMATION_FRAME_COUNT = static_cast<size_t>(AnimFrame::Count);
