// Engine/Core/Utils/TableParser.h

#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <sstream>
#include <iostream>

#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/FileLoader.h"

// 제네릭 타입 T 사용 함수는 예외적으로 헤더 파일에 작성
namespace TableParser {
	// filepath, table, mapping 람다 함수 입력 필요. mapping 함수는 const std::vector<std::string>&를 받아 std::optional<std::pair<std::string, T>>를 반환하는 함수가 아닐 경우 본문에서 에러 발생 우려
	// mapping 함수 반환 값에는 반드시 std::string 형태의 키 값이 있어야 하며 return std::pair값에 포함시킬 것
	template<typename T, typename MappingFunc>
	bool ParseTsvTable(const std::string& filepath, std::unordered_map<std::string, T>& mappingTable, MappingFunc mapping) {	
		std::vector<std::string> outLines;
		FileLoader::LoadFile(filepath, outLines);

		bool headerSkipped = false;
		int lineCount = 0;
		std::vector<int> erroredLines;
		for (const std::string& rawLine : outLines) {
			std::string checkLine = StringUtils::Trim(rawLine);
			lineCount++;

			// 유효한 라인인지 검증
			if (checkLine.empty() || checkLine[0] == '#') { 
				continue;
			}

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
			
			if (!mapped) {
				erroredLines.push_back(lineCount);
				continue;
			}
			mappingTable[mapped->first] = mapped->second;
		}

		if (erroredLines.size() > 0) {
			std::cerr << filepath << " 로드 중 문제 발생. 다음은 오류 라인 목록" << std::endl;
			for (int i = 0; i < erroredLines.size(); i++) {
				std::cerr << "\t- " << (erroredLines[i]) << "번 라인" << std::endl;
			}
			return false;
		}
		return true;
	}
}