// BeingParser.cpp
#include "Engine/Modules/Being/BeingParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/TableParser.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace BeingParser {

void BeingParser::ParseBeingConfig(const std::string& filepath,
	std::unordered_map<std::string, BeingMapData>& beingTable){
	const int COLSIZE = 11;

	auto mapping = [COLSIZE](const std::vector<std::string>& cols)->std::optional<BeingMapData> {
		if (cols.size() != COLSIZE) {
			std::cerr << "[Being Parser Error: Colsize ºÒÀÏÄ¡" << std::endl;
			return std::nullopt;
		}
		std::string id = StringUtils::Trim(cols[0]);
		std::string type = StringUtils::Trim(cols[1]);
		std::string aiType = StringUtils::Trim(cols[2]);
		std::string mapID = StringUtils::Trim(cols[3]);
		float posX = std::stof(StringUtils::Trim(cols[4]));
		float posY = std::stof(StringUtils::Trim(cols[5]));
		float mass = std::stof(StringUtils::Trim(cols[6]));
		float radius = std::stof(StringUtils::Trim(cols[7]));
		std::string textureKey = StringUtils::Trim(cols[8]);
		int dialogID = std::stoi(StringUtils::Trim(cols[9]));
		std::string uiName = StringUtils::Trim(cols[10]);

		return BeingMapData{
			id,
			type,
			aiType, 
			mapID,
			posX,
			posY,
			mass,
			radius,
			textureKey,
			dialogID,
			uiName
		};};

		TableParser::ParseTsvTable(filepath, beingTable, mapping);
	}
}