// Game/Modules/Combat/CombatParser.cpp

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>

#include "Game/Modules/Combat/CombatParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/TableParser.h"



void CombatParser::ParseAttackConfig(const std::string& filepath, std::unordered_map<std::string, AttackMapData>& atkTable) {
	const int COLSIZE = 5;

	auto mapping = [COLSIZE](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, AttackMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[ParseAttckConfig Error]: COLSIZE 불일치" << std::endl;
			return std::nullopt;
		};
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
		return std::pair{ id,
			AttackMapData{ id,
			currentAtk,
			modifiedAtk,
			currentCritRate,
			modifiedCritRate,
			currentCritRatio,
			modifiedCritRatio,
			currentSpeed,
			modifiedSpeed } }; };

	TableParser::ParseTsvTable(filepath, atkTable, mapping);
	
}

void CombatParser::ParseDefenseConfig(const std::string& filepath, std::unordered_map<std::string, DefenseMapData>& defTable) {
	// 테이블의 colSize는 전용 파서에서 명시해야 함
	const int COLSIZE = 2;

	auto mapping = [COLSIZE](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, DefenseMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "COLSIZE 불일치" << std::endl;
			return std::nullopt;
		}
		// 해당 테이블에서 키 값은 beingId가 될 것.
		std::string beingId = StringUtils::Trim(cols[0]);
		float defaultDef = std::stof(StringUtils::Trim(cols[1]));
		float baseDef = defaultDef + 0.0f;
		float modifiedDef = baseDef + 0.0f;

		return std::pair{ beingId, DefenseMapData{ beingId, baseDef, modifiedDef } };
		};

	TableParser::ParseTsvTable(filepath, defTable, mapping);
}


void CombatParser::ParseElementalConfig(const std::string& filepath, std::unordered_map <std::string, ElementalMapData>& elementTable) {
	
	const int COLSIZE = 11;

	auto mapping = [COLSIZE](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, ElementalMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[CombatParser Error]: COLSIZE 불일치" << std::endl;
			return std::nullopt;
		}
		ElementalMapData elementData;
		std::string beingId = StringUtils::Trim(cols[0]);
		elementData.beingId = beingId;
		for (int i = 0; i < 5; i++) {
			elementData.baseElementAtkRatio[i] = std::stof(StringUtils::Trim(cols[1 + i]));
			elementData.modifiedElementAtkRatio[i] = elementData.baseElementAtkRatio[i] + 0.0f;	// 임시 modifying 값
		}
		for (int i = 0; i < 5; i++) {
			elementData.baseElementDefRatio[i] = std::stof(StringUtils::Trim(cols[6 + i]));
			elementData.modifiedElementDefRatio[i] = elementData.baseElementDefRatio[i] + 0.0f;	// 임시 modifying 값
		}

		return std::pair{
			beingId,
			elementData
		};};
		
	TableParser::ParseTsvTable(filepath, elementTable, mapping);
}

