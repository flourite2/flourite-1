// MotionTypes.h

#pragma once

// 애니메이션 구현을 위한 방향 매핑
enum class Direction {
    Down = 0,
    Up = 1,
    Left = 2,
    Right = 3,
    Count = 4
};

// 애니메이션 프레임 매핑
enum class AnimFrame {
    Stand = 0,
    Walk1 = 1,
    Walk2 = 2,
    Count = 3
};