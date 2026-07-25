// RenderParser.cpp

#include "Engine/Modules/Render/RenderParser.h"
#include "Engine/Core/Utils/ConfigParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace RenderParser {

    // 헤더에 extern(또는 static)으로 선언된 변수의 실제 메모리 할당
    std::vector<int> mapData;
    int mapCols = 0;
    int mapRows = 0;

    // ★ 새로 추가된 스폰 포인트 배열 메모리 할당
    std::vector<SpawnPointData> spawnPoints;

    void LoadMapCSV(const std::string& filepath) {
        mapData.clear();
        mapCols = 0;
        mapRows = 0;

        std::ifstream file(filepath);
        if (!file.is_open()) { /* 기존 에러 처리 */ return; }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            int colCount = 0;
            while (std::getline(ss, token, ',')) {
                if (token.empty() || token == "\r") continue;
                mapData.push_back(std::stoi(token));
                colCount++;
            }
            if (colCount > 0) {
                mapCols = colCount; // 마지막 행 기준 (모든 행이 같다고 가정)
                mapRows++;
            }
        }
    }

    void ParseRenderConfig(std::string filepath, std::unordered_map<std::string, RenderMapData>& renderTable) {
        std::string extension = "";
        std::vector<std::string> outLines;
        FileLoader::LoadFile(filepath, outLines, extension);

        if (extension == ".tsv") {
            bool headerFlag = false;
            for (const std::string& rawLine : outLines) {
                std::string checkLine = StringUtils::Trim(rawLine);
                if (checkLine.empty() || checkLine[0] == '#')
                    continue;

                if (!headerFlag) {
                    headerFlag = true;
                    continue;
                }

                std::stringstream ss(rawLine);
                std::string rawCategory, rawName, rawFilename;
                if (std::getline(ss, rawCategory, '\t') &&
                    std::getline(ss, rawName, '\t') &&
                    std::getline(ss, rawFilename, '\t')) {

                    std::string category = StringUtils::Trim(rawCategory);
                    std::string name = StringUtils::Trim(rawName);
                    std::string filename = StringUtils::Trim(rawFilename);

                    if (!category.empty() && !name.empty() && !filename.empty()) {
                        renderTable[name] = RenderMapData{ category, name, filename };
                        std::cout << "[tsv Debug] Loaded Render Mapping: Category='" << category << "', Name='" << name << "', Filename='" << filename << "'" << std::endl;
                    }
                }
            }
        }
    }

    std::vector<std::vector<int>> LoadSpriteCSV(const std::string& filepath) {
        std::vector<std::vector<int>> animTable;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "[RenderParser] Failed to open sprite CSV: " << filepath << std::endl;
            return animTable;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<int> row;
            std::stringstream ss(line);
            std::string cell;

            // 쉼표(,)를 기준으로 문자열을 잘라 숫자로 변환
            while (std::getline(ss, cell, ',')) {
                // 혹시 모를 공백이나 \r 같은 제어 문자 제거 (trim 효과)
                cell.erase(std::remove_if(cell.begin(), cell.end(), ::isspace), cell.end());

                if (!cell.empty()) {
                    row.push_back(std::stoi(cell));
                }
            }
            if (!row.empty()) {
                animTable.push_back(row);
            }
        }
        return animTable;
    }

    void LoadMapJSON(const std::string& filepath, EntityManager& registry) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cout << "[Error] JSON 맵 파일을 열 수 없습니다: " << filepath << "\n";
            return;
        }

        json j;
        file >> j; // 파싱 완료

        // 1. 맵의 전체 가로/세로 타일 개수 자동 갱신
        mapCols = j["width"].get<int>();
        mapRows = j["height"].get<int>();

        mapData.clear();
        spawnPoints.clear(); // ★ 맵 로드 시 스폰 데이터도 초기화

        // 맵 전체에 적용될 스케일 팩터 (객체와 타일 모두 동일하게 적용)
        float mapScale = ConfigParser::GetFloat("Map.Scale", 6.0f);

        // 2. 모든 레이어 순회
        for (const auto& layer : j["layers"]) {
            std::string type = layer["type"].get<std::string>();
            std::string layerName = layer.contains("name") ? layer["name"].get<std::string>() : "";

            // [A] 타일 레이어 (눈에 보이는 맵)
            if (type == "tilelayer") {
                for (int tileID : layer["data"]) {
                    // Tiled의 JSON 타일 ID는 1부터 시작하므로 (0은 빈 공간), 1을 빼줍니다.
                    mapData.push_back(tileID > 0 ? tileID - 1 : -1);
                }
                std::cout << "[Map Debug] 타일 레이어 로드 완료. (총 " << mapData.size() << " 개)\n";
            }

            // [B] 객체 레이어 (물리 충돌체 및 스폰 포인트)
            else if (type == "objectgroup") {

                // [B-1] 스폰 포인트 레이어 파싱 부분
                if (layerName == "Spawn Point") {
                    for (const auto& obj : layer["objects"]) {
                        SpawnPointData sp;
                        sp.entityID = obj.contains("name") ? obj["name"].get<std::string>() : "Unknown";

                        // ★ 추가: JSON에 gid가 있으면 가져오고 없으면 0
                        sp.gid = obj.contains("gid") ? obj["gid"].get<int>() : 0;

                        float h = obj.contains("height") ? obj["height"].get<float>() : 0.0f;
                        sp.posX = obj["x"].get<float>() * mapScale;
                        sp.posY = (obj["y"].get<float>() - h) * mapScale;

                        spawnPoints.push_back(sp);
                    }
                    std::cout << "[Map Debug] 스폰 포인트 레이어 로드 완료. (총 " << spawnPoints.size() << " 개)\n";
                }
                // [B-2] 기존 벽(Walls) 물리 레이어 파싱
                else {
                    for (const auto& obj : layer["objects"]) {
                        std::string objClass = obj.contains("class") ? obj["class"].get<std::string>() :
                            (obj.contains("type") ? obj["type"].get<std::string>() : "");

                        // 레이어 이름이 Walls Layer거나 객체 클래스가 Wall/Solid인 경우
                        if (objClass == "Wall" || objClass == "Solid" || layerName == "Walls Layer") {

                            float tWidth = obj["width"].get<float>() * mapScale;
                            float tHeight = obj["height"].get<float>() * mapScale;
                            float tX = obj["x"].get<float>() * mapScale;
                            float tY = obj["y"].get<float>() * mapScale;

                            // ECS 규격(중심점 좌표)으로 변환
                            float centerX = tX + (tWidth / 2.0f);
                            float centerY = tY + (tHeight / 2.0f);

                            // ECS에 정적 물리 엔티티(벽) 추가
                            Entity wall = registry.CreateEntity();
                            registry.AddTransform(wall, TransformComponent{ Vector2D(centerX, centerY), Vector2D(centerX, centerY), Vector2D(1.0f, 1.0f) });

                            PhysicsComponent wallPhys;
                            wallPhys.mass = 0.0f; // 무한대 질량 (벽)
                            wallPhys.isSleeping = false; // ← 명시적으로 추가
                            wallPhys.collider = Collider::CreateAABBCollider(tWidth, tHeight);
                            registry.AddPhysics(wall, wallPhys);
                        }
                    }
                    std::cout << "[Map Debug] 스케일(x" << mapScale << ")이 적용된 물리 충돌 레이어 로드 완료.\n";
                }
            }
        }
    }
}