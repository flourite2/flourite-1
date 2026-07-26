// Engine/Modules/Stat/StatParser.cpp

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Engine/Modules/Stat/StatParser.h"
#include "Engine/ECS/Components/Stat/StatComponents.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"

namespace StatParser {
	void ParseStatConfig(const std::string& filepath, std::unordered_map<std::string, StatMapData>& statTable) {
		std::vector<std::string> outLines;
		FileLoader::LoadFile(filepath, outLines);

		for (const auto& line : outLines) {
			std::cout << "[RAW LINE] " << line << std::endl;
		}

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

			const int COLSIZE = 5;

			if (cols.size() < COLSIZE) {  
				std::cerr << "[StatParser] 필수 컬럼 부족, 건너뜀: " << rawLine << "\n";
				continue;
			}

			StatMapData data;
			data.id = StringUtils::Trim(cols[0]);
			std::cout << "[StatParser] 로드: id=" << data.id << std::endl;
			float maxHp = std::stof(StringUtils::Trim(cols[1]));
			float currentHp = maxHp - 0.0f;	// 현재 변동치 파일이 없으므로 전부 하드코딩
			data.hpMapData = { maxHp , currentHp };
			float maxMp = std::stof(StringUtils::Trim(cols[2]));
			float currentMp = maxMp - 0.0f;
			data.mpMapData = { maxMp , currentMp };
			float defaultExp = std::stof(StringUtils::Trim(cols[3]));
			float currentExp = defaultExp + 0.0;
			int defaultLevel = std::stoi(StringUtils::Trim(cols[4]));
			int currentLevel = defaultLevel + 0;
			float maxExp = 10.0f + ((float) currentLevel * 10.0f);
			int maxLevel = 25;
			data.LevelMapData = { currentLevel, maxLevel, currentExp, maxExp };
			
			statTable[data.id] = data;



			std::cout << "[StatParser] 로드" << std::endl;
		}
	}
}