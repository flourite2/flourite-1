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
	void ParseStatConfig(const std::string& filepath, std::unordered_map<std::string, StatMapData>& statTable) {
		const int COLSIZE = 5;

		auto mapping = [COLSIZE](const std::vector<std::string>& cols) -> std::optional<std::pair<std::string, StatMapData>> {
			if (cols.size() != COLSIZE) {
				std::cerr << "[StatParser Error]: COLSIZE 불일치" << std::endl;
				return std::nullopt;
			}

			std::string beingId = StringUtils::Trim(cols[0]);

			float maxHp = std::stof(StringUtils::Trim(cols[1]));
			float currentHp = maxHp - 0.0f;	// 현재 변동치 파일이 없으므로 전부 하드코딩
			HPMapData hpMapData = { maxHp , currentHp };

			float maxMp = std::stof(StringUtils::Trim(cols[2]));
			float currentMp = maxMp - 0.0f;
			MPMapData mpMapData = { maxMp , currentMp };

			// 임시 값
			float defaultExp = std::stof(StringUtils::Trim(cols[3]));
			float currentExp = defaultExp + 10.0f;
			int defaultLevel = std::stoi(StringUtils::Trim(cols[4]));
			int currentLevel = defaultLevel + 0;
			float maxExp = 10.0f + ((float)currentLevel * 10.0f);
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
		TableParser::ParseTsvTable(filepath, statTable, mapping);

	}

}