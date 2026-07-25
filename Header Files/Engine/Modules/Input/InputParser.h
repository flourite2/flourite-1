// InputParser.h

#pragma once
#include <string>
#include <unordered_map>

// 파싱된 입력 데이터를 담는 구조체 (구현부에서 사용됨)
struct InputMapData {
    std::string action;
    std::string input;
};

namespace InputParser {
    // tsv 또는 ini 파일을 읽어 inputTable 맵에 매핑합니다.
    void ParseInputConfig(std::string filepath, std::unordered_map<std::string, InputMapData>& inputTable);
}