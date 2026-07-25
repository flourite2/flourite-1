// Game/Modules/Combat/CombatParser.h

#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include <optional>
#include <functional>

struct AttackMapData {
	std::string id;

	float baseAtk;
	float modifiedAtk;
	float baseCritRate;
	float modifiedCritRate;
	float baseCritRatio;
	float modifiedCritRatio;
	float baseSpeed;
	float modifiedSpeed;
};
struct DefenseMapData {
	std::string id;

	float baseDef;
	float modifiedDef;

};

struct ElementalMapData {
	std::string id;

	std::array<float, 5> baseElementAtkRatio;
	std::array<float, 5> modifiedElementAtkRatio;

	std::array<float, 5> baseElementDefRatio;
	std::array<float, 5> modifiedElementDefRatio;
};

namespace CombatParser {
	void ParseAttackConfig(const std::string& filepath, std::unordered_map<std::string, AttackMapData>& atkTable);
	void ParseDefenseConfig(const std::string& filepath, std::unordered_map<std::string, DefenseMapData>& defTable);
	void ParseElementalConfig(const std::string& filepath, std::unordered_map <std::string, ElementalMapData>& eleTable);
	template<typename T>
	void ParseTsvTable(const std::string& filepath, std::unordered_map<std::string, T>& mappingTable, std::function<std::optional<T>(const std::vector<std::string>&)> mapping);
}