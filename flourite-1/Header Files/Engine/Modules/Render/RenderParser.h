// RenderParser.h

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/ECS/Components/Render/RenderComponent.h"
#include "Engine/Core/Utils/json.hpp"
#include "Engine/ECS/Entities/EntityManager.h" // ★ 충돌체 엔티티 조립을 위해 추가

using json = nlohmann::json;

struct SpawnPointData {
    std::string entityID;
    float posX;
    float posY;
    int gid; // ★ 추가: Tiled에서 부여한 타일 이미지 번호
};

namespace RenderParser {
    // 헤더에는 다른 파일들이 참조할 수 있도록 extern 또는 static 구조로 선언만 합니다.
    extern std::vector<int> mapData;
    extern int mapCols;
    extern int mapRows;
    extern std::vector<SpawnPointData> spawnPoints; // 중복 정의 방지를 위한 extern 선언

    void LoadMapCSV(const std::string& filepath);
    std::vector<std::vector<int>> LoadSpriteCSV(const std::string& filepath);
    void LoadMapJSON(const std::string& filepath, EntityManager& registry);
}