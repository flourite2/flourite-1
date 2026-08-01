// Game/ECS/Components/CombatComponents.h

#pragma once

#include <array>
#include <string>

enum class ElementType {
	HOT,
	COLD,
	NORMAL,
	LIGHT,
	DARK
};


struct ElementalComponent {
	std::array<float, 5> baseElementAtkRatio;
	std::array<float, 5> modifiedElementAtkRatio;

	std::array<float, 5> baseElementDefRatio;
	std::array<float, 5> modifiedElementDefRatio;
};

struct AttackComponent {
	float baseAtk;
	float modifiedAtk;
	float baseCritRate;
	float modifiedCritRate;
	float baseCritRatio;
	float modifiedCritRatio;
	float baseSpeed;
	float modifiedSpeed;
};

struct DefenseComponent {
	float baseDef;
	float modifiedDef;
};

