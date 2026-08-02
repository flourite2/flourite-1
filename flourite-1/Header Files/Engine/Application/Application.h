// Application.h

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <SFML/Graphics.hpp>
#include <filesystem>

#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/ECS/Systems/PhysicsSystem2D.h"
#include "Engine/ECS/Systems/RenderSystem2D.h"
#include "Engine/ECS/Systems/FSMSystem.h"
#include "Engine/Modules/Render/Renderer.h"
#include "Engine/Modules/Input/InputManager.h"
#include "Engine/Modules/Being/BeingParser.h"
#include "Game/Maps/TileMap.h"
#include "Engine/Modules/Dialog/DialogData.h"
#include "Engine/Modules/Dialog/DialogStatus.h"
#include "Engine/ECS/Components/Stat/StatComponents.h"
#include "Game/ECS/Components/CombatComponents.h"
#include "Engine/Modules/Stat/StatParser.h"
#include "Game/Modules/Combat/CombatParser.h"

enum class GameState {
    PLAYING,      // 평소 이동 상태
    DIALOG,       // 대화 중 (물리 정지, UI 활성화)
    PAUSE_MENU    // 메뉴 창 열림 (물리 정지, 메뉴 UI 활성화)
};
inline GameState currentState = GameState::PLAYING;

struct MenuItem {
    int id;
    std::string name;
};

struct MenuDetail {
    int id;
	std::string description;    // 이거는 나중에 html같이 자유롭게 표현 가능한 것으로 대체해야 함
};

class Application {
public:
    Application();
    ~Application();

    bool Init();
    void Run();

private:
    // ★ 4단계 초기화 모듈
    bool InitConfigAndSystems();
    bool InitAssets();
    bool InitEntities();
    bool InitEvents();

    void ProcessEvents();
    void Update(float dt);
    void Render(float alpha);

private:
    EntityManager registry;
    std::optional<InputManager> inputManager;
    std::optional<Renderer> renderer;
    std::optional<PhysicsSystem2D> physicsSystem;
    std::optional<RenderSystem2D> renderSystem;
    FSMSystem fsmSystem;

    bool isRunning = false;
    int resX = 1920, resY = 1080;
    float interactRange = 50.0f;
    float halfW = 0.f, halfH = 0.f, clampMaxX = 0.f, clampMaxY = 0.f;
    float mapW = 0.f, mapH = 0.f; // ★ 여러 Init() 간 데이터 공유를 위한 멤버 변수화
    float padding = 150.f;

    // 물리 연산 시 사용할 변수
    float targetVelocity = 0.0f;

    // ★ [새로 추가할 부분: 범용 리소스 컨테이너]
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
    TileMap myTileMap;
    std::unordered_map<std::string, std::vector<std::vector<int>>> animTables;
    std::unordered_map<std::string, InputMapData> inputTable;

    
    std::unordered_map<std::string, BeingMapData> beingTable;

    sf::View cameraView;
    PhysicsSystem2D::CullRect cullRect;
    Vector2D camPos;

    // 메뉴 관련 멤버 변수 추가
    std::vector<MenuItem> menuItems;
	std::vector<MenuDetail> menuDetails;
    int currentMenuIndex = 0;
	int currentMenuDetailIndex = 0;

    // 스탯
    std::unordered_map<std::string, StatMapData> statTable;
    std::unordered_map<std::string, AttackMapData> atkTable;
    std::unordered_map<std::string, DefenseMapData> defTable;
    std::unordered_map<std::string, ElementalMapData> eleTable;

    // 경로 관련
    std::string projectDirName;
    std::filesystem::path projectDir;
};