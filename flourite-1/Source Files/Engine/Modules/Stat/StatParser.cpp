// Engine/Modules/Stat/StatParser.cpp

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <optional>
#include "Engine/Modules/Stat/StatParser.h"
#include "Engine/ECS/Components/Stat/StatComponents.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/TableParser.h"

namespace StatParser {
	bool ParseStatConfig(const std::string& filepath, std::unordered_map<std::string, StatMapData>& statTable) {
		constexpr int COLSIZE = 5;

		auto mapping = [](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, StatMapData>> {
			if (cols.size() != COLSIZE) {
				std::cerr << "[ParseStatConfig() 실행 중 에러]: COLSIZE 불일치\n[" << "기대값: " << COLSIZE << " | 실제값: " << cols.size() << "]" << std::endl;
				return std::nullopt;
			}

			std::string beingId = StringUtils::Trim(cols[0]);

			auto maxHp = StringUtils::TryParseFloat(StringUtils::Trim(cols[1]), (beingId+"'s maxHp"));
			if (!maxHp) return std::nullopt;
			float currentHp = *maxHp;	// 현재 변동치 파일이 없으므로 전부 하드코딩
			HPMapData hpMapData = { *maxHp , currentHp };

			auto maxMp = StringUtils::TryParseFloat(StringUtils::Trim(cols[2]), (beingId + "'s maxMp"));
			if (!maxMp) return std::nullopt;
			float currentMp = *maxMp;
			MPMapData mpMapData = { *maxMp , currentMp };

			// 임시 값들: 시각화를 위해 임시 매직 넘버 기입
			auto defaultExp = StringUtils::TryParseFloat(StringUtils::Trim(cols[3]), (beingId + "'s defaultExp"));
			if (!defaultExp) return std::nullopt;
			float currentExp = *defaultExp + 10.0f;
			auto defaultLevel = StringUtils::TryParseInt(StringUtils::Trim(cols[4]), (beingId + "'s defaultLevel"));
			if (!defaultLevel) return std::nullopt;
			int currentLevel = *defaultLevel + 0;
			float maxExp = 10.0f + (static_cast<float>(currentLevel) * 10.0f);
			int maxLevel = 25;
			LevelMapData lvMapData = { currentLevel, maxLevel, currentExp, maxExp };

			return std::pair{
				beingId,
				StatMapData{beingId,
				hpMapData,
				mpMapData,
				lvMapData
				}
			};
			};
		return TableParser::ParseTsvTable(filepath, statTable, mapping);

	}

}