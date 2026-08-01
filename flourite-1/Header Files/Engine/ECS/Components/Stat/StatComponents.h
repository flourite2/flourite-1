// Engine/ECS/Components/Stat/StatComponents.h

#pragma once

struct HPComponent {
	float hp;
	float maxHp;
};
struct MPComponent {
	float mp;
	float maxMp;
};
struct LevelComponent {
	int level;
	int maxLevel;
	float exp;
	float maxExp;
};
