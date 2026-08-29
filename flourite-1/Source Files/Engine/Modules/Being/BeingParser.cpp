// BeingParser.cpp
#include "Engine/Modules/Being/BeingParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/TableParser.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <utility>

bool BeingParser::ParseBeingConfig(const std::string& filepath,
std::unordered_map<std::string, BeingMapData>& beingTable){
constexpr int COLSIZE = 11;

auto mapping = [](const std::vector<std::string>& cols)->std::optional<std::pair<std::string, BeingMapData>> {
	if (cols.size() != COLSIZE) {
		std::cerr << "[ParseBeingConfig() 실행 중 에러]: COLSIZE 불일치\n[" << "기대값: " << COLSIZE << " | 실제값: " << cols.size() << "]" << std::endl;
		return std::nullopt;
	}
	std::string id = StringUtils::Trim(cols[0]);
	std::string type = StringUtils::Trim(cols[1]);
	std::string aiType = StringUtils::Trim(cols[2]);
	std::string mapID = StringUtils::Trim(cols[3]);

	auto posX = StringUtils::TryParseFloat(StringUtils::Trim(cols[4]), (id + "'s posX"));
	if (!posX) return std::nullopt;
	auto posY = StringUtils::TryParseFloat(StringUtils::Trim(cols[5]), (id + "'s posY"));
	if (!posY) return std::nullopt;
	auto mass = StringUtils::TryParseFloat(StringUtils::Trim(cols[6]), (id + "'s mass"));
	if (!mass) return std::nullopt;
	auto radius = StringUtils::TryParseFloat(StringUtils::Trim(cols[7]), (id + "'s radius"));
	if (!radius) return std::nullopt;
	std::string textureKey = StringUtils::Trim(cols[8]);
	auto dialogID = StringUtils::TryParseInt(StringUtils::Trim(cols[9]), (id + "'s dialogID"));
	if (!dialogID) return std::nullopt;
	std::string uiName = StringUtils::Trim(cols[10]);

	return std::pair { id,
		BeingMapData{
		id,
		type,
		aiType,
		mapID,
		*posX,
		*posY,
		*mass,
		*radius,
		textureKey,
		*dialogID,
		uiName
	}}; };

	return TableParser::ParseTsvTable(filepath, beingTable, mapping);
}
