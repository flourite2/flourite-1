// Game/ECS/Components/CombatComponents.h

#pragma once

#include <array>

enum class ElementType {
	HOT,
	COLD,
	NORMAL,
	LIGHT,
	DARK,
	COUNT
};

constexpr size_t ELEMENTCOUNT = static_cast<size_t>(ElementType::COUNT);

struct ElementalComponent {
	std::array<float, ELEMENTCOUNT> baseElementAtkRatio;
	std::array<float, ELEMENTCOUNT> modifiedElementAtkRatio;

	std::array<float, ELEMENTCOUNT> baseElementDefRatio;
	std::array<float, ELEMENTCOUNT> modifiedElementDefRatio;
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

