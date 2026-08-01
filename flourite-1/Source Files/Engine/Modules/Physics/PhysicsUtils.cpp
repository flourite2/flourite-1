// PhysicsUtils.cpp

#include "Engine/Modules/Physics/PhysicsUtils.h"

Vector2D PhysicsUtils::CalculateForce(Vector2D direction, float velocity, float mass, float damping) {
	return direction.Normalize() * velocity * mass * damping;
}