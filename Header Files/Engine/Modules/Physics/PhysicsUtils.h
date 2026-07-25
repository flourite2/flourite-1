// Engine/Modules/Physics/PhysicsUtils.h

#pragma once

#include "Engine/Core/Math/Vector2D.h"

namespace PhysicsUtils {
	// 해당 함수 호출 시 방향 정보 Normalize 필요 없음(내부에서 실행)
	Vector2D CalculateForce(Vector2D direction, float velocity, float mass, float damping);
}