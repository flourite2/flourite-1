// Engine/Modules/Stat/StatParser.h

#pragma once

#include <string>
#include <unordered_map>

struct HPMapData {
	float hp;
	float maxHp;
};
struct MPMapData {
	float mp;
	float maxMp;
};
struct LevelMapData {
	int level;
	int maxLevel;
	float exp;
	float maxExp;
};

struct StatMapData {
	HPMapData hpMapData;
	MPMapData mpMapData;
	LevelMapData LevelMapData;
	std::string id;
};

namespace StatParser {
	// Stat 관련 파싱 함수 선언
	void ParseStatConfig(const std::string& filepath, std::unordered_map<std::string, StatMapData>& statTable);
}