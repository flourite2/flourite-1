// Engine/Core/Utils/TableParser.h

#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <sstream>

#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/FileLoader.h"

// 제네릭 타입 T 사용 함수는 예외적으로 헤더 파일에 작성
namespace TableParser {
	// filepath, table, mapping 람다 함수 입력 필요. mapping 함수는 const std::vector<std::string>&를 받아 std::optional<T>를 반환하는 함수가 아닐 경우 본문에서 에러 발생 우려
	// T 객체는 id를 반드시 가져야 함
	template<typename T, typename MappingFunc>
	void ParseTsvTable(const std::string& filepath, std::unordered_map<std::string, T>& mappingTable, MappingFunc mapping) {	
		std::vector<std::string> outLines;
		FileLoader::LoadFile(filepath, outLines);

		bool headerSkipped = false;
		for (const std::string& rawLine : outLines) {
			std::string checkLine = StringUtils::Trim(rawLine);
			// 유효한 라인인지 검증
			if (checkLine.empty() || checkLine[0] == '#') continue;

			// 유효한 첫 라인 스킵
			if (!headerSkipped) {
				headerSkipped = true;
				continue;
			}

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
}