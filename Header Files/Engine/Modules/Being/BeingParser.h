// BeingParser.h

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// 엔진에서 조립될 '존재(Being)'의 청사진 데이터
struct BeingMapData {
    std::string id;
    std::string type;
    std::string aiType;   // 신규 추가
    std::string mapID;
    float posX;
    float posY;
    float mass;
    float radius;
    std::string textureKey;
    int dialogID = -1;
    std::string uiName; // 이 줄을 추가하세요!
};

namespace BeingParser {
    // Being은 외부의 beingTable을 인자로 받아 채워 넣는 OOP 방식을 유지합니다.
    void ParseBeingConfig(const std::string& filepath, std::unordered_map<std::string, BeingMapData>& beingTable);
}