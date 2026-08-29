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



bool CombatParser::ParseAttackConfig(const std::string& filepath, std::unordered_map<std::string, AttackMapData>& atkTable) {
	constexpr int COLSIZE = 5;

	auto mapping = [](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, AttackMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[ParseAttackConfig() 실행 중 에러]: COLSIZE 불일치\n[" << "기대값: " << COLSIZE << " | 실제값: " << cols.size() << "]" << std::endl;
			return std::nullopt;
		};
		std::string id = StringUtils::Trim(cols[0]);

		auto defaultAtk = StringUtils::TryParseFloat(StringUtils::Trim(cols[1]), (id+"'s defaultAtk"));
		if (!defaultAtk) return std::nullopt;
		float currentAtk = *defaultAtk;
		float modifiedAtk = ((currentAtk + 0.0f) * 1.0f) + 0.0f;	// 곱연산 이전 추가 공격력, 공격력 배율, 최후 추가 공격력 고려한 수식

		auto defaultCritRate = StringUtils::TryParseFloat(StringUtils::Trim(cols[2]), (id + "'s defaultCritRate"));
		if (!defaultCritRate) return std::nullopt;
		float currentCritRate = *defaultCritRate;
		float modifiedCritRate = ((currentCritRate + 0.0f) * 1.0f) + 0.0f;

		auto defaultCritRatio = StringUtils::TryParseFloat(StringUtils::Trim(cols[3]), (id + "'s defaultCritRatio"));
		if (!defaultCritRatio) return std::nullopt;
		float currentCritRatio = *defaultCritRatio;
		float modifiedCritRatio = ((currentCritRatio + 0.0f) * 1.0f) + 0.0f;

		auto defaultSpeed = StringUtils::TryParseFloat(StringUtils::Trim(cols[4]), (id + "'s defaultSpeed"));
		if (!defaultSpeed) return std::nullopt;
		float currentSpeed = *defaultSpeed;
		float modifiedSpeed = ((currentSpeed + 0.0f) * 1.0f) + 0.0f;

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

	return TableParser::ParseTsvTable(filepath, atkTable, mapping);
	
}

bool CombatParser::ParseDefenseConfig(const std::string& filepath, std::unordered_map<std::string, DefenseMapData>& defTable) {
	// 테이블의 colSize는 전용 파서에서 명시해야 함
	constexpr int COLSIZE = 2;

	auto mapping = [](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, DefenseMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[ParseDefenseConfig() 실행 중 에러]: COLSIZE 불일치\n[" << "기대값: " << COLSIZE << " | 실제값: " << cols.size() << "]" << std::endl;
			return std::nullopt;
		}
		// 해당 테이블에서 키 값은 beingId가 될 것.
		std::string beingId = StringUtils::Trim(cols[0]);

		auto defaultDef = StringUtils::TryParseFloat(StringUtils::Trim(cols[1]), (beingId + "'s defaultDef"));
		if (!defaultDef) return std::nullopt;

		float baseDef = *defaultDef;
		float modifiedDef = ((baseDef + 0.0f) * 1.0f) + 0.0f;

		return std::pair{ beingId, DefenseMapData{ beingId, baseDef, modifiedDef } };
		};

	return TableParser::ParseTsvTable(filepath, defTable, mapping);
}


bool CombatParser::ParseElementalConfig(const std::string& filepath, std::unordered_map <std::string, ElementalMapData>& elementTable) {
	
	constexpr size_t KEYCOLS = 1;
	constexpr size_t BLOCKCOUNT = 2;

	constexpr size_t COLSIZE = KEYCOLS + (ELEMENTCOUNT * BLOCKCOUNT);

	auto mapping = [](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, ElementalMapData>> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[ParseElementalConfig() 실행 중 에러]: COLSIZE 불일치\n[" << "기대값: " << COLSIZE << " | 실제값: " << cols.size() << "]" << std::endl;
			return std::nullopt;
		}
		ElementalMapData elementData;
		std::string beingId = StringUtils::Trim(cols[0]);
		elementData.beingId = beingId;
		for (size_t i = 0; i < ELEMENTCOUNT; i++) {
			// 공격 배율
			auto baseEAR = StringUtils::TryParseFloat(StringUtils::Trim(cols[KEYCOLS + i]), (beingId + "'s baseElementAtkRatio[" + std::to_string(i) + "]"));
			if (!baseEAR) return std::nullopt;
			elementData.baseElementAtkRatio[i] = *baseEAR;
			elementData.modifiedElementAtkRatio[i] = ((elementData.baseElementAtkRatio[i] +0.0f) * 1.0f) +0.0f;	// 임시 modifying 값

			// 방어 배율
			auto baseEDR = StringUtils::TryParseFloat(StringUtils::Trim(cols[KEYCOLS + ELEMENTCOUNT + i]), (beingId + "'s baseElementDefRatio[" + std::to_string(i) + "]"));
			if (!baseEDR) return std::nullopt;
			elementData.baseElementDefRatio[i] = *baseEDR;
			elementData.modifiedElementDefRatio[i] = ((elementData.baseElementDefRatio[i] +0.0f) * 1.0f) + 0.0f;	// 임시 modifying 값
		}

		return std::pair{
			beingId,
			elementData
		};};
		
	return TableParser::ParseTsvTable(filepath, elementTable, mapping);
}
