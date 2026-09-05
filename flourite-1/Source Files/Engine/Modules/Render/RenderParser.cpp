// RenderParser.cpp

#include "Engine/Modules/Render/RenderParser.h"
#include "Engine/Core/Utils/ConfigParser.h"
#include "Engine/Modules/Render/MotionTypes.h"
#include <fstream>
#include <sstream>
#include <iostream>

// 헤더에 extern(또는 static)으로 선언된 변수의 실제 메모리 할당
std::vector<int> RenderParser::mapData;
int RenderParser::mapCols = 0;
int RenderParser::mapRows = 0;

// ★ 새로 추가된 스폰 포인트 배열 메모리 할당
std::vector<SpawnPointData> RenderParser::spawnPoints;    

std::optional<std::vector<std::vector<int>>> RenderParser::LoadSpriteCSV(const std::string& filepath) {
    std::vector<std::vector<int>> animTable;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "[RenderParser::LoadSpriteCSV() 실행 중 에러]: 스프라이트 CSV 파일 오픈 실패. 파일명: " << filepath << std::endl;
        return std::nullopt;
    }

    int rowCount = 0;
    // 반환값을 결정하는 값
    bool errored = false;

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        // 혹시 모를 공백이나 \r 같은 제어 문자 제거 (trim 효과)
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty()) break;
        std::stringstream ss(line);
        std::string cell;
        
        rowCount++;
        int colCount = 0;
        // 쉼표(,)를 기준으로 문자열을 잘라 숫자로 변환
        while (std::getline(ss, cell, ',')) {
            colCount++;

            std::string targetName = std::to_string(rowCount) + "번 줄 " + std::to_string(colCount) + "번째 요소";

            auto parsed = StringUtils::TryParseInt(cell, targetName);
            if (!parsed) {
                errored = true;

                continue;
            }
            row.push_back(*parsed);
        }
        if (row.size() != DIRECTION_COUNT) {
            std::cerr << "[RenderParser::LoadSpriteCSV() 실행 중 에러]:\n위치: " << filepath << "의 " << rowCount << "번 줄\n열 개수 불일치\n기대값: " << DIRECTION_COUNT << ", 실제값: " << row.size() << std::endl;
            errored = true;

            continue;
        }
        animTable.push_back(row);
    }
    if (animTable.size() != ANIMATION_FRAME_COUNT) {
        std::cerr << "[RenderParser::LoadSpriteCSV() 실행 중 에러]:\n파일명: " << filepath << "\n행 개수 불일치\n기대값: " << ANIMATION_FRAME_COUNT << ", 실제값: " << animTable.size() << std::endl;

        errored = true;
    }

    if (errored) return std::nullopt;
    return animTable;
}

bool RenderParser::LoadMapJSON(const std::string& filepath, EntityManager& registry) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[Error] JSON 맵 파일을 열 수 없습니다: " << filepath << "\n";
        return false;
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
    return true;
}
