// Game/Modules/Combat/CombatParser.cpp

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Game/Modules/Combat/CombatParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"



void CombatParser::ParseAttackConfig(const std::string& filepath, std::unordered_map<std::string, AttackMapData>& atkTable) {

	std::vector<std::string> outLines;
	std::string extentions;
	FileLoader::LoadFile(filepath, outLines, extentions);

	bool headerSkipped = false;
	for (const std::string& rawLine : outLines) {
		std::string checkLine = StringUtils::Trim(rawLine);

		// 첫 줄 스킵
		if (!headerSkipped) {
			headerSkipped = true;
			continue;
		}

		if (checkLine.empty() || checkLine[0] == '#') continue;

		std::stringstream ss(rawLine);
		std::vector<std::string> cols;
		std::string token;

		while (std::getline(ss, token, '\t')) {
			cols.push_back(token);
		}

		const int COLSIZE = 4;
		if (cols.size() < COLSIZE) {
			std::cerr << "[CombatParser] 필수 컬럼 부족, 건너뜀: " << rawLine << "\n";
			continue;
		}

		AttackMapData data;
		std::string id = StringUtils::Trim(cols[0]);
		float defaultAtk = std::stof(StringUtils::Trim(cols[1]));
		float currentAtk = defaultAtk + 0.0f;
		float modifiedAtk = ((currentAtk + 0.0f) * 1.0f) + 1.0f;	// 곱연산 이전 추가 공격력, 공격력 배율, 최후 추가 공격력 고려한 수식
		float defaultCritRate = std::stof(StringUtils::Trim(cols[2]));
		float currentCritRate = defaultCritRate + 0.0f;
		float modifiedCritRate = ((currentCritRate + 0.0f) * 1.0f) + 1.0f;
		float defaultCritRatio = std::stof(StringUtils::Trim(cols[3]));
		float currentCritRatio = defaultCritRatio + 0.0f;
		float modifiedCritRatio = ((currentCritRatio + 0.0f) * 1.0f) + 1.0f;
		float defaultSpeed = std::stof(StringUtils::Trim(cols[4]));
		float currentSpeed = defaultSpeed + 0.0f;
		float modifiedSpeed = ((currentSpeed + 0.0f) * 1.0f) + 1.0f;
		data = { id, currentAtk, modifiedAtk, currentCritRate, modifiedCritRate, currentCritRatio, modifiedCritRatio, currentSpeed, modifiedSpeed };

		atkTable[data.id] = data;
	}
}

template<typename T>
void CombatParser::ParseTsvTable(const std::string& filepath, std::unordered_map<std::string, T>& mappingTable, std::function<std::optional<T>(const std::vector<std::string>&)> mapping) {
	std::vector<std::string> outLines;
	std::string extentions;
	FileLoader::LoadFile(filepath, outLines, extentions);

	bool headerSkipped = false;
	for (const std::string& rawLine : outLines) {
		std::string checkLine = StringUtils::Trim(rawLine);

		// 첫 줄 스킵
		if (!headerSkipped) {
			headerSkipped = true;
			continue;
		}

		if (checkLine.empty() || checkLine[0] == '#') continue;

		std::stringstream ss(rawLine);
		std::vector<std::string> cols;
		std::string token;

		while (std::getline(ss, token, '\t')) {
			cols.push_back(token);
		}

		auto mapped = mapping(cols);
		if (mapped) {
			mappingTable[mapped->id] = *mapped;
		}
	}
}
void CombatParser::ParseDefenseConfig(const std::string& filepath, std::unordered_map<std::string, DefenseMapData>& defTable) {

	const int COLSIZE = 2;

	auto mapping = [COLSIZE](const std::vector<std::string>& cols) -> std::optional<DefenseMapData> {
		if (cols.size() != COLSIZE) {
			std::cerr << "COLSIZE 불일치" << std::endl;
			return std::nullopt;
		}
		std::string id = StringUtils::Trim(cols[0]);
		float defaultDef = std::stof(StringUtils::Trim(cols[1]));
		float baseDef = defaultDef + 0.0f;
		float modifiedDef = baseDef + 0.0f;

		return DefenseMapData{ id, baseDef, modifiedDef };
		};

	ParseTsvTable<DefenseMapData>(filepath, defTable, mapping);
}


void CombatParser::ParseElementalConfig(const std::string& filepath, std::unordered_map <std::string, ElementalMapData>& elementTable) {
	std::vector<std::string> outLines;
	std::string extentions;
	FileLoader::LoadFile(filepath, outLines, extentions);

	bool headerSkipped = false;
	for (const std::string& rawLine : outLines) {
		std::string checkLine = StringUtils::Trim(rawLine);

		// 첫 줄 스킵
		if (!headerSkipped) {
			headerSkipped = true;
			continue;
		}

		if (checkLine.empty() || checkLine[0] == '#') continue;

		std::stringstream ss(rawLine);
		std::vector<std::string> cols;
		std::string token;

		while (std::getline(ss, token, '\t')) {
			cols.push_back(token);
		}

		const int COLSIZE = 11;
		if (cols.size() < COLSIZE) {
			std::cerr << "[CombatParser] 필수 컬럼 부족, 건너뜀: " << rawLine << "\n";
			continue;
		}

		ElementalMapData data;
		data.id = StringUtils::Trim(cols[0]);

		for (int i = 0; i < 5; i++) {
			data.baseElementAtkRatio[i] = std::stof(StringUtils::Trim(cols[1 + i]));
		}
		for (int i = 0; i < 5; i++) {
			data.baseElementDefRatio[i] = std::stof(StringUtils::Trim(cols[6 + i]));
		}

		elementTable[data.id] = data;
	}
}

