// Application.cpp

#include "Engine/Application/Application.h"
#include "Engine/Core/Time/Timer.h"
#include "Engine/Core/Time/FixedStepAccumulator.h"
#include "Engine/Core/Utils/PerformanceTracker.h"
#include "Engine/Core/Utils/ConfigParser.h"
#include "Engine/Modules/Input/InputParser.h"
#include "Engine/Modules/Input/InputKeys.h"
#include "Engine/Modules/Render/RenderParser.h"
#include "Engine/Modules/Event/EventHandler.h"
#include "Engine/Modules/Event/InputEvent.h"
#include "Engine/Modules/Dialog/DialogStatus.h"
#include "Engine/Modules/Dialog/DialogParser.h"
#include "Engine/Modules/Physics/PhysicsUtils.h"

#include "Engine/Core/Utils/DirectoryHelper.h"

#include "Engine/ECS/Components/Stat/StatComponents.h"
#include "Game/ECS/Components/CombatComponents.h"

// ★ 철저하게 분리된 외부 모듈 인클루드
#include "Engine/Modules/UI/UIUtils.h"             
#include "Engine/Modules/Being/BeingFactory.h"     
#include "Engine/Modules/Render/AnimationUpdater.h"

#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include <sstream>
#include <iostream>
#include <random>
#include <windows.h>

#include "Engine/Modules/UI/Menu.h"


Application::Application() {}
Application::~Application() {}

bool Application::Init() {
    if (!InitConfigAndSystems()) return false;
    if (!InitAssets()) return false;
    if (!InitEntities()) return false;
    if (!InitEvents()) return false;

    isRunning = true;

    // 메뉴 선택 박스(임시 하드코딩)
    // 일단 메뉴 목록은 정보, 종료만 테스트

    menuItems.push_back({ 0, "information" });
    menuItems.push_back({ 1, "exit" });

    menuDetails.push_back({ 0, "" });
    menuDetails.push_back({ 1, "Exit the game" });

    return true;
}

bool Application::InitConfigAndSystems() {
    projectDirName = "flourite-1";
    projectDir = DirectoryHelper::GetProjectDirectory(projectDirName);
    if (projectDir.empty()) return false;
    std::filesystem::current_path(projectDir);

    ConfigParser::LoadConfig("Resource Files/Config/engine_config.ini");

    resX = ConfigParser::GetInt("Window.Width", 1920);
    resY = ConfigParser::GetInt("Window.Height", 1080);
    interactRange = ConfigParser::GetFloat("Physics.InteractRange", 50.0f);
    std::cout << "[Interact Range Check]: " << interactRange;

    InputParser::ParseInputConfig("Resource Files/Config/input_config.ini", inputTable);

    StatParser::ParseStatConfig("Resource Files/Being/Stat/base_stat_table.tsv", statTable);
    CombatParser::ParseAttackConfig("Resource Files/Being/Stat/base_attack_table.tsv", atkTable);
    CombatParser::ParseDefenseConfig("Resource Files/Being/Stat/base_defense_table.tsv", defTable);
    
    for (const auto& [key, data] : defTable) {
        std::cout << "key=[" << key << "] baseDef=" << data.baseDef << std::endl;
    }

    CombatParser::ParseElementalConfig("Resource Files/Being/Stat/base_elemental_table.tsv", eleTable);    

    renderer.emplace(ConfigParser::GetInt("Window.MaxFPS", 144), resX, resY);
    renderSystem.emplace(renderer.value());
    physicsSystem.emplace(40, 40);

    halfW = resX / 2.0f;
    halfH = resY / 2.0f;

    ConfigParser::LoadConfig("Resource Files/Being/being_config.ini");
    targetVelocity = ConfigParser::GetFloat("Movement.DefaultVelocity", 300.0f);
    float damping = ConfigParser::GetFloat("Movement.DefaultDamping", 8.0f);

    inputManager.emplace(inputTable);

    return true;
}

bool Application::InitAssets() {
    // 1. 매니페스트 파일 로드 (이 경로 하나만 최후의 하드코딩/INI로 남깁니다)
    std::string manifestPath = ConfigParser::GetString("Paths.AssetManifest", "Resource Files/Config/asset_manifest.tsv");

    std::vector<std::string> lines;
    FileLoader::LoadFile(manifestPath, lines);

    if (lines.empty()) {
        std::cerr << "[Fatal Error] 에셋 매니페스트 파일을 찾을 수 없습니다: " << manifestPath << "\n";
        return false;
    }

    // 2. 동적 에셋 로딩 루프
    bool headerSkipped = false;
    for (const auto& line : lines) {
        std::string raw = StringUtils::Trim(line);
        if (raw.empty() || raw[0] == '#') continue;
        if (!headerSkipped) { headerSkipped = true; continue; }

        std::stringstream ss(raw);
        std::string id, type, path;
        std::getline(ss, id, '\t');
        std::getline(ss, type, '\t');
        std::getline(ss, path, '\t');

        id = StringUtils::Trim(id);
        type = StringUtils::Trim(type);
        path = StringUtils::Trim(path);

        // ★ Type에 따라 알아서 딕셔너리에 꽂아 넣음!
        if (type == "Texture") {
            if (!textures[id].loadFromFile(path)) std::cerr << "[Warning] 텍스처 로드 실패: " << path << "\n";
        }
        else if (type == "Font") {
            if (!fonts[id].openFromFile(path)) std::cerr << "[Warning] 폰트 로드 실패: " << path << "\n";
        }
        else if (type == "SpriteCSV") {
            animTables[id] = RenderParser::LoadSpriteCSV(path);
        }
        else if (type == "BeingTSV") {
            BeingParser::ParseBeingConfig(path, beingTable);
        }
        else if (type == "DialogTSV") {
            DialogParser::ParseDialogConfig(path);
        }
        else if (type == "MapJSON") {
            RenderParser::LoadMapJSON(path, registry);
        }
    }

    // 3. 맵 크기 및 물리 바운더리 세팅
    int tileSize = ConfigParser::GetInt("Map.TileSize", 16);
    float mapScale = ConfigParser::GetFloat("Map.Scale", 6.0f);

    mapW = static_cast<float>(RenderParser::mapCols) * tileSize * mapScale;
    mapH = static_cast<float>(RenderParser::mapRows) * tileSize * mapScale;

    physicsSystem->SetMapBounds(mapW, mapH);
    clampMaxX = std::max(halfW, mapW - halfW);
    clampMaxY = std::max(halfH, mapH - halfH);

    // ★ 딕셔너리에서 텍스처를 꺼내서 사용
    myTileMap.Load(&textures["Tilemap_packed"], tileSize, ConfigParser::GetInt("Map.SheetColumns", 27), RenderParser::mapCols, RenderParser::mapRows, mapScale, RenderParser::mapData);

    std::cout << "[System] 에셋 매니페스트 동적 로딩 완료.\n";
    return true;
}

bool Application::InitEntities() {
    // 1. Tiled 맵 기반 스폰

    BeingFactory::SpawnDefault sd = { ConfigParser::GetFloat("Spawn.DefaultDamping", 8.0f) };
    for (const auto& sp : RenderParser::spawnPoints) {
        if (beingTable.find(sp.entityID) == beingTable.end()) continue;
        const auto& bData = beingTable[sp.entityID];

        int tileIDToUse = 0;
        bool isCharacter = (bData.type == "Player" || bData.type == "player" || bData.type == "NPC" || bData.type == "npc");

        if (isCharacter && animTables.find(bData.textureKey) != animTables.end()) {
            tileIDToUse = animTables[bData.textureKey][0][0];
        }
        else if (sp.gid > 0) {
            tileIDToUse = sp.gid - 1;
        }

        // ★ 모듈화된 팩토리 호출
        Entity e = BeingFactory::SpawnBeing(registry, bData, &textures["Tilemap_packed"], tileIDToUse, sd, sp.posX, sp.posY);

        if (bData.type == "Player" || bData.type == "player") registry.signatures[e].set(COMP_PLAYER_TAG);
        else if (bData.type == "NPC" || bData.type == "npc") registry.signatures[e].set(COMP_NPC_TAG);

        if (bData.aiType != "NONE" && !bData.aiType.empty()) {
            FSMComponent fsm;
            fsm.currentState = FSMState::IDLE;
            fsm.dialogID = bData.dialogID;
            if (bData.aiType == "STATIONARY") fsm.aiType = AIType::STATIONARY;
            else if (bData.aiType == "WAYPOINT") fsm.aiType = AIType::WAYPOINT;
            else if (bData.aiType == "CHATTERBOX") fsm.aiType = AIType::CHATTERBOX;
            registry.AddFSM(e, fsm);
        }
    }

    // 2. 스트레스 테스트 더미 스폰
    int dummyCount = ConfigParser::GetInt("System.StressTestDummyCount", 10);
    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> posDist(0.0f, mapW);
    int dummyTileID = 251;
    if (animTables.find("player_sd") != animTables.end()) dummyTileID = animTables["player_sd"][0][0];

    for (int i = 0; i < dummyCount; ++i) {
        BeingFactory::SpawnBeing(registry, beingTable["Dummy"], &textures["Tilemap_packed"], dummyTileID, sd, posDist(rng), posDist(rng));
    }

    // 3. 플레이어 스폰 검증
    bool hasPlayer = false;
    for (Entity i = 0; i < registry.GetEntityCapacity(); ++i) {
        if (registry.signatures[i].test(COMP_PLAYER_TAG)) {
            hasPlayer = true;
            camPos = registry.transforms[i].position;
            break;
        }
    }
    if (!hasPlayer) {
        std::cerr << "[Fatal Error] 플레이어가 월드에 스폰되지 않았습니다!\n";
        return false;
    }

    // 4. 초기 물리 안정화 (60틱)
    PhysicsSystem2D::CullRect initCull = { 0.f, 0.f, mapW, mapH };
    for (int i = 0; i < 60; ++i) {
        physicsSystem->FixedUpdate(registry, 0.016f, initCull);
    }

    return true;
}

bool Application::InitEvents() {
    auto interactKey = InputKeys::ToSFMLKey(inputTable["Interact"].input).value_or(sf::Keyboard::Key::Unknown);
    if (interactKey != sf::Keyboard::Key::Unknown) {
        std::cout << "[Interact] 키 입력 감지, interactRange=" << interactRange << std::endl;
        inputManager->RegisterEvent(interactKey, [this]() {
            if (g_DialogStatus.isSpeaking) {
                if (!g_DialogStatus.isTypingFinished) {
                    g_DialogStatus.Skip();
                }
                else {
                    float maxWidth = static_cast<float>(resX) - 140.0f;
                    if (g_DialogStatus.isWaitingForChoice) {
                        int targetID = g_DialogStatus.currentDialog->choices[g_DialogStatus.currentChoiceIndex].second;
                        if (targetID == -1) { 
                            g_DialogStatus.isSpeaking = false;
                            currentState = GameState::PLAYING;

                            std::cout << "대화끝" << std::endl;
                        }
                        
                        else g_DialogStatus.Start(targetID, fonts["MainFont"], 24, maxWidth);
                    }
                    else {
                        int nextID = g_DialogStatus.currentDialog->nextDialogID;
                        if (nextID == -1) {
                            g_DialogStatus.isSpeaking = false;
                            currentState = GameState::PLAYING;

                            std::cout << "대화끝" << std::endl;
                        }
                        else g_DialogStatus.Start(nextID, fonts["MainFont"], 24, maxWidth);
                    }
                }
                return;
            }

            Entity playerEntity = registry.GetEntityCapacity();
            uint32_t maxEntities = registry.GetEntityCapacity();
            for (Entity i = 0; i < maxEntities; ++i) {
                if (registry.signatures[i].test(COMP_PLAYER_TAG)) { playerEntity = i; break; }
            }
            if (playerEntity == maxEntities) return;

            auto& pTrans = registry.transforms[playerEntity];
            auto& pPhys = registry.physics[playerEntity];

            for (Entity i = 0; i < maxEntities; ++i) {
                if (i == playerEntity) continue;
                if (!registry.signatures[i].test(COMP_TRANSFORM) || !registry.signatures[i].test(COMP_PHYSICS) || !registry.signatures[i].test(COMP_FSM)) continue;

                auto& nTrans = registry.transforms[i];
                auto& nPhys = registry.physics[i];
                auto& nFsm = registry.fsms[i];

                if (Event::InteractionByPlayer(pTrans, pPhys, nTrans, nPhys, interactRange)) {
                    std::cout << "[Interact] 상호작용 판정 성공, entity=" << i << " dialogID=" << nFsm.dialogID << std::endl;
                    if (nFsm.dialogID != -1) {
                        float maxWidth = static_cast<float>(resX) - 140.0f;
                        g_DialogStatus.Start(nFsm.dialogID, fonts["MainFont"], 24, maxWidth);
                        nFsm.currentState = FSMState::CONVERSATION;
                        nPhys.velocity = Vector2D(0, 0);
                        nTrans.facedDirection = pTrans.facedDirection * -1.0f;
                        AnimationUpdater::UpdateAnimation(registry, i, 0.0f, animTables);
                        currentState = GameState::DIALOG;

                        std::cout << "대화시작" << std::endl;
                    }
                    break;
                }
            }
            });
    }

    auto uiUpKey = InputKeys::ToSFMLKey(inputTable["UI_Up"].input).value_or(sf::Keyboard::Key::Unknown);
    auto uiDownKey = InputKeys::ToSFMLKey(inputTable["UI_Down"].input).value_or(sf::Keyboard::Key::Unknown);

        
    if (uiUpKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(uiUpKey, []() {
            if (currentState == GameState::DIALOG) {
                if (g_DialogStatus.isWaitingForChoice) {
                    int maxChoices = g_DialogStatus.currentDialog->choices.size();
                    g_DialogStatus.currentChoiceIndex--;
                    if (g_DialogStatus.currentChoiceIndex < 0) g_DialogStatus.currentChoiceIndex = maxChoices - 1;
                }
            }
            });
    }

    if (uiDownKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(uiDownKey, []() {
            if (currentState == GameState::DIALOG) {
                if (g_DialogStatus.isWaitingForChoice) {
                    int maxChoices = g_DialogStatus.currentDialog->choices.size();
                    g_DialogStatus.currentChoiceIndex++;
                    if (g_DialogStatus.currentChoiceIndex >= maxChoices) g_DialogStatus.currentChoiceIndex = 0;
                }
            }
            });
    }

    auto menuKey = InputKeys::ToSFMLKey(inputTable["Menu"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuUpKey = InputKeys::ToSFMLKey(inputTable["Menu_Up"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuLeftKey = InputKeys::ToSFMLKey(inputTable["Menu_Left"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuDownKey = InputKeys::ToSFMLKey(inputTable["Menu_Down"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuRightKey = InputKeys::ToSFMLKey(inputTable["Menu_Right"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuSelectKey = InputKeys::ToSFMLKey(inputTable["Menu_Select"].input).value_or(sf::Keyboard::Key::Unknown);
    auto menuCancleKey = InputKeys::ToSFMLKey(inputTable["Menu_Cancle"].input).value_or(sf::Keyboard::Key::Unknown);

    if (menuKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(menuKey, []() {
            if (currentState == GameState::PLAYING) {
                // 메뉴 열기
                Menu::OpenMenu();

                // 상태 변경
                currentState = GameState::PAUSE_MENU;

                // Move 관련 동작 안 하게 하기, Menu 관련 키만 작동하도록 하기.

            }
            else if (currentState == GameState::PAUSE_MENU) {
                // 메뉴 닫기
                Menu::CloseMenu();

                // 상태 변경
                currentState = GameState::PLAYING;

                // Open 때와 반대로 Move 관련 동작 하게 하기, Menu 관련 키 작동하지 않도록 하기.
            }
            else {
                std::cout << "현재 작동 불가한 기능(대화 중 메뉴 조작 불가)" << std::endl;
            }
            });
    }

    // 미완성
    
    if (menuUpKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(menuUpKey, [this]() {
            if (currentState == GameState::PAUSE_MENU)
                currentMenuIndex = std::max(0, currentMenuIndex-1);
        });
    }
    if (menuDownKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(menuDownKey, [this]() {
            if (currentState == GameState::PAUSE_MENU)
				
                currentMenuIndex = std::min(static_cast<int>(menuItems.size()) - 1, currentMenuIndex + 1);
        });
    }
    if(menuSelectKey != sf::Keyboard::Key::Unknown) {
        inputManager->RegisterEvent(menuSelectKey, [this]() {
            if (currentState == GameState::PAUSE_MENU) {
                if (currentMenuIndex == 0) {
                    std::cout << "정보 선택됨" << std::endl;
					currentMenuDetailIndex = 0;
                }
                else if (currentMenuIndex == 1) {
                    std::cout << "종료 선택됨" << std::endl;
					currentMenuDetailIndex = 1;
                }
            }
        });
		
    }

    return true;
}

void Application::Run() {
    Timer timer;
    FixedStepAccumulator fsa(ConfigParser::GetInt("Physics.Hz", 100));
    const float physicsDT = 1.0f / static_cast<float>(ConfigParser::GetInt("Physics.Hz", 100));
    PerformanceTracker perfTracker(ConfigParser::GetInt("System.PerfTrackerSamples", 1000));

    while (renderer->IsOpen() && isRunning) {
        timer.Tick();
        double dtDouble = timer.GetDeltaTimeSeconds();
        if (currentState == GameState::PLAYING) fsa.Update(timer.GetDeltaTime());

        if (currentState == GameState::DIALOG) {
            g_DialogStatus.Update(physicsDT);  // 타이프라이터만 별도로
        }
        

        ProcessEvents();

        const int repeat = fsa.GetRepeatCount();
        for (int i = 0; i < repeat; i++) {
            if (currentState == GameState::PLAYING) {
                Update(physicsDT);  // 물리+FSM은 기존처럼
            }
        }

        float dtFloat = static_cast<float>(dtDouble);
        uint32_t maxEntities = registry.GetEntityCapacity();
        if (currentState == GameState::PLAYING) {
            for (Entity i = 0; i < maxEntities; ++i) {
                if (registry.signatures[i].test(COMP_PLAYER_TAG) || registry.signatures[i].test(COMP_NPC_TAG)) {
                    AnimationUpdater::UpdateAnimation(registry, i, dtFloat, animTables);
                }
            }
        }
        Render(fsa.GetAlpha());

        perfTracker.RecordFrame(timer.GetDeltaTime(), 0, renderer->GetAndResetDrawCalls(), renderer->GetAndResetTextureSwaps());
        if (perfTracker.IsSampleFull()) perfTracker.PrintStatistics();
    }
}

void Application::ProcessEvents() {
    Event::EventHandler::ProcessEvents(renderer.value(), inputManager.value());
    Vector2D direction(0.0f, 0.0f);
    
    inputManager->UpdateMoveDirection(direction);
    if (g_DialogStatus.isSpeaking) direction = Vector2D(0.0f, 0.0f);

    for (Entity i = 0; i < registry.GetEntityCapacity(); ++i) {
        if (registry.signatures[i].test(COMP_PLAYER_TAG)) {
            registry.physics[i].force = PhysicsUtils::CalculateForce(direction, targetVelocity, registry.physics[i].mass, registry.physics[i].damping);
            
            if (g_DialogStatus.isSpeaking) registry.physics[i].velocity = Vector2D(0.0f, 0.0f);
            break;
        }
    }
}

void Application::Update(float dt) {
    cullRect.left = camPos.x - halfW - padding;
    cullRect.right = camPos.x + halfW + padding;
    cullRect.top = camPos.y - halfH - padding;
    cullRect.bottom = camPos.y + halfH + padding;

    
    fsmSystem.Update(registry, dt, targetVelocity); // ★ FSM 중복 호출 버그 수정
    physicsSystem->FixedUpdate(registry, dt, cullRect);
}

void Application::Render(float alpha) {
    for (Entity i = 0; i < registry.GetEntityCapacity(); ++i) {
        if (registry.signatures[i].test(COMP_PLAYER_TAG)) {
            camPos = registry.transforms[i].position;
            break;
        }
    }

    camPos.x = std::clamp(camPos.x, halfW, clampMaxX);
    camPos.y = std::clamp(camPos.y, halfH, clampMaxY);

    cameraView.setCenter(sf::Vector2f(camPos.x, camPos.y));
    cameraView.setSize(sf::Vector2f(static_cast<float>(resX), static_cast<float>(resY)));
    renderer->SetView(cameraView);

    renderer->Clear(sf::Color(50, 50, 50));
    renderer->Draw(myTileMap);
    renderSystem->Render(registry, alpha);

    if (g_DialogStatus.isSpeaking && g_DialogStatus.currentDialog != nullptr) {
        sf::View uiView;
        uiView.setSize({ (float)resX, (float)resY });
        uiView.setCenter({ (float)resX / 2.0f, (float)resY / 2.0f });
        renderer->SetView(uiView);

        float winW = static_cast<float>(resX);
        float winH = static_cast<float>(resY);
        float padding = 40.0f;
        sf::FloatRect boxBounds;
        boxBounds.size = { winW - (padding * 2.0f), 200.0f };
        boxBounds.position = { padding, winH - boxBounds.size.y - padding };

        // ★ 모듈화된 UI 그리기 호출
        UIUtils::DrawNineSlice(&renderer.value(), textures["UIBox"], boxBounds, 12.0f, sf::Color(255, 255, 255));

        std::string speakerID = g_DialogStatus.currentDialog->speakerID;
        std::string uiName = (beingTable.find(speakerID) != beingTable.end()) ? beingTable[speakerID].uiName : speakerID;

        sf::Text nameText(fonts["MainFont"], sf::String::fromUtf8(uiName.begin(), uiName.end()), 28);
        nameText.setFillColor(sf::Color(50, 50, 50));
        nameText.setStyle(sf::Text::Bold);
        nameText.setPosition({ boxBounds.position.x + 30.0f, boxBounds.position.y + 15.0f });
        renderer->Draw(nameText);

        sf::Sprite dividerSprite(textures["UIDivider"]);
        float dividerTargetWidth = boxBounds.size.x - 60.0f;
        float dividerScaleX = dividerTargetWidth / (float)textures["UIDivider"].getSize().x;
        dividerSprite.setScale({ dividerScaleX, 1.0f });
        dividerSprite.setPosition({ boxBounds.position.x + 30.0f, boxBounds.position.y + 55.0f });
        dividerSprite.setColor(sf::Color(150, 150, 150, 200));
        renderer->Draw(dividerSprite);

        sf::String slicedText = g_DialogStatus.wrappedText.substring(0, g_DialogStatus.visibleCharCount);
        sf::Text dialogText(fonts["MainFont"], slicedText, 24);
        dialogText.setFillColor(sf::Color::Black);
        dialogText.setPosition({ boxBounds.position.x + 30.0f, boxBounds.position.y + 75.0f });
        renderer->Draw(dialogText);

        if (g_DialogStatus.isWaitingForChoice) {
            const auto& choices = g_DialogStatus.currentDialog->choices;
            float choiceBoxW = 400.0f;
            float choiceBoxH = 60.0f;
            float spacing = 10.0f;
            float startX = boxBounds.position.x + boxBounds.size.x - choiceBoxW;
            float startY = boxBounds.position.y - (choices.size() * (choiceBoxH + spacing));

            for (size_t i = 0; i < choices.size(); ++i) {
                sf::FloatRect cBounds;
                cBounds.position = { startX, startY + (i * (choiceBoxH + spacing)) };
                cBounds.size = { choiceBoxW, choiceBoxH };

                UIUtils::DrawNineSlice(&renderer.value(), textures["UIBox"], cBounds, 12.0f, sf::Color(255, 255, 255));

                sf::Text cText(fonts["MainFont"], sf::String::fromUtf8(choices[i].first.begin(), choices[i].first.end()), 24);
                if (i == g_DialogStatus.currentChoiceIndex) {
                    cText.setFillColor(sf::Color(0, 100, 255));
                    cText.setStyle(sf::Text::Bold);
                }
                else {
                    cText.setFillColor(sf::Color(100, 100, 100));
                    cText.setStyle(sf::Text::Regular);
                }
                cText.setPosition({ cBounds.position.x + 20.0f, cBounds.position.y + 15.0f });
                renderer->Draw(cText);
            }
        }
    }

    if (currentState == GameState::PAUSE_MENU) {
        // 1. 뷰 세팅 
        sf::View menuView;
        menuView.setSize({ (float)resX, (float)resY });
        menuView.setCenter({ (float)resX / 2.0f, (float)resY / 2.0f });
        renderer->SetView(menuView);

        // 2. 수학적 변수 세팅
        float totalW = 1600.0f;
        float totalH = 900.0f;
        float startX = (resX - totalW) / 2.0f;
        float startY = (resY - totalH) / 2.0f;

        // 3. 메뉴 박스 렌더링

        // 3. 1. 왼쪽 박스
        // 기본 렌더
        sf::FloatRect leftBounds;
		leftBounds.position = { startX, startY };
		leftBounds.size = { totalW/4.0f, totalH };

        float itemsMargine = 10.0f;
		float itemsW = leftBounds.size.x - (itemsMargine * 2.0f);
		float itemsH = 50.0f;

		float startItemX = leftBounds.position.x + itemsMargine;
		float startItemY = leftBounds.position.y + itemsMargine;

        UIUtils::DrawNineSlice(&renderer.value(), textures["MenuBox"], leftBounds, 12.0f, sf::Color(255, 255, 255));

        sf::FloatRect menuItemBounds;
        for (size_t i = 0; i < menuItems.size(); i++) {
			menuItemBounds.position = { startItemX, startItemY + i * (itemsH + itemsMargine) };
            menuItemBounds.size = { itemsW, itemsH };
            sf::Text itemText(fonts["MainFont"], sf::String::fromUtf8(menuItems[i].name.begin(), menuItems[i].name.end()), 24);
            if (i == currentMenuIndex) {
                UIUtils::DrawNineSlice(&renderer.value(), textures["MenuBox"], menuItemBounds, 12.0f, sf::Color(100, 200, 255));
                itemText.setFillColor(sf::Color(255, 255, 255));
                itemText.setStyle(sf::Text::Bold);
            }
            else {
                UIUtils::DrawNineSlice(&renderer.value(), textures["MenuBox"], menuItemBounds, 12.0f, sf::Color(255, 255, 255));
                itemText.setFillColor(sf::Color(50, 50, 50));
                itemText.setStyle(sf::Text::Bold);
            }
            itemText.setPosition({ menuItemBounds.position.x + itemsMargine, menuItemBounds.position.y + (itemsH - itemText.getLocalBounds().position.y) / 2.0f - 10.0f });
            renderer->Draw(itemText);
        }


        // 3. 2. 오른쪽 박스
        sf::FloatRect rightBounds;
		rightBounds.position = { startX + totalW/4.0f, startY };
		rightBounds.size = { totalW * 3.0f/4.0f, totalH };
		UIUtils::DrawNineSlice(&renderer.value(), textures["MenuBox"], rightBounds, 12.0f, sf::Color(255, 255, 255));

		float detailMargin = 20.0f;
		float detailTextW = rightBounds.size.x - (detailMargin * 2.0f);
		float detailTextH = 50.0f;

		float startDetailX = rightBounds.position.x + detailMargin;
        float startDetailY = rightBounds.position.y + detailMargin;
		
        for (size_t i = 0; i < menuDetails.size(); i++) {
            if (menuDetails[i].id == currentMenuDetailIndex) {
                if (currentMenuDetailIndex == 0) {
                    // 시각화할 수치(비율) 계산
                    HPComponent playerHpComp = { statTable["player"].hpMapData.hp, statTable["player"].hpMapData.maxHp };
                    MPComponent playerMpComp = { statTable["player"].mpMapData.mp, statTable["player"].mpMapData.maxMp };
                    LevelComponent playerLvComp = { statTable["player"].LevelMapData.level,statTable["player"].LevelMapData.maxLevel,statTable["player"].LevelMapData.exp,statTable["player"].LevelMapData.maxExp };
                    AttackComponent playerAtkComp = { atkTable["player"].baseAtk, atkTable["player"].modifiedAtk,atkTable["player"].baseCritRate, atkTable["player"].modifiedCritRate,atkTable["player"].baseCritRatio, atkTable["player"].modifiedCritRatio,atkTable["player"].baseSpeed, atkTable["player"].modifiedSpeed };
                    DefenseComponent playerDefComp = { defTable["player"].baseDef, defTable["player"].modifiedDef };
                    ElementalComponent playerEleComp = { eleTable ["player"] .baseElementAtkRatio, eleTable["player"].modifiedElementAtkRatio, eleTable["player"].baseElementDefRatio , eleTable["player"].modifiedElementDefRatio };

                    // 잠시 제대로 불러와졌는지 콘솔에서 확인용 코드
                    std::cout << "[playerAtkComp Test]\n\nbaseAtk: " << playerAtkComp.baseAtk << "\nmodifiedAtk: " << playerAtkComp.modifiedAtk << "\nbaseCritRate" << playerAtkComp.baseCritRate
                        << "\nmodifiedCritRate: " << playerAtkComp.modifiedCritRate << "\nbaseCritRatio" << playerAtkComp.modifiedCritRatio << "\nbaseSpeed: " << playerAtkComp.baseSpeed << "\nmodifiedSpeed" << playerAtkComp.modifiedSpeed << std::endl;
                    std::cout << "[playerDefComp Test]\n\nbaseDef: " << playerDefComp.baseDef << "\nmodifiedDef: " << playerDefComp.modifiedDef << std::endl;
                    


					float hpRatio = std::max(playerHpComp.hp / playerHpComp.maxHp, 0.0f);
					float mpRatio = std::max(playerMpComp.mp / playerMpComp.maxMp, 0.0f);
					float expRatio = std::max(playerLvComp.exp / playerLvComp.maxExp, 0.0f);

                    int statInfoMargine = 24;

					sf::Text hpText(fonts["MainFont"], "HP: " + std::to_string(playerHpComp.hp) + "/" + std::to_string(playerHpComp.maxHp), statInfoMargine);
                    sf::FloatRect baseHPBar;
                    sf::FloatRect currentHPBar;

                    sf::Text mpText(fonts["MainFont"], "MP: " + std::to_string(playerMpComp.mp) + "/" + std::to_string(playerMpComp.maxMp), statInfoMargine);
					sf::FloatRect baseMPBar;
					sf::FloatRect currentMPBar;

					sf::Text levelText(fonts["MainFont"], "Level: " + std::to_string(playerLvComp.level), statInfoMargine);
                    sf::Text expText(fonts["MainFont"], "EXP: " + std::to_string(playerLvComp.exp) + "/" + std::to_string(playerLvComp.maxExp), statInfoMargine);
					sf::FloatRect baseEXPBar;
                    sf::FloatRect currentEXPBar;

					hpText.setFillColor(sf::Color(50, 50, 50));
					hpText.setStyle(sf::Text::Regular);
					hpText.setPosition({ startDetailX, startDetailY });
					baseHPBar.position = { startDetailX, startDetailY + 30.0f };
					baseHPBar.size = { detailTextW, 20.0f };
                    if (hpRatio > 0.0f) {
                        currentHPBar.position = baseHPBar.position;
                        currentHPBar.size = { detailTextW * hpRatio, 20.0f };
                    }

					float mpBarY = baseHPBar.position.y + 50.0f;
                    mpText.setFillColor(sf::Color(50, 50, 50));
                    mpText.setStyle(sf::Text::Regular);
                    mpText.setPosition({ startDetailX, mpBarY });
                    baseMPBar.position = { startDetailX, mpBarY + 30.0f };
                    baseMPBar.size = { detailTextW, 20.0f };
                    if (mpRatio > 0.0f) {
                        currentMPBar.position = baseMPBar.position;
                        currentMPBar.size = { detailTextW * mpRatio, 20.0f };
                    }

					levelText.setFillColor(sf::Color(50, 50, 50));
					levelText.setStyle(sf::Text::Regular);
					levelText.setPosition({ startDetailX, baseMPBar.position.y + 50.0f });

					float expBarY = levelText.getPosition().y + 50.0f;
                    expText.setFillColor(sf::Color(50, 50, 50));
                    expText.setStyle(sf::Text::Regular);
                    expText.setPosition({ startDetailX, expBarY });
                    baseEXPBar.position = { startDetailX, expBarY + 30.0f };
                    baseEXPBar.size = { detailTextW, 20.0f };
                    if (expRatio > 0.0f) {
                        currentEXPBar.position = baseEXPBar.position;
                        currentEXPBar.size = { detailTextW * expRatio, 20.0f };
                    }
					renderer->Draw(hpText);
                    UIUtils::DrawNineSlice(&renderer.value(), textures["BaseBar"], baseHPBar, 20.0f, sf::Color::White);
					UIUtils::DrawNineSlice(&renderer.value(), textures["HPBar"], currentHPBar, 20.0f, sf::Color::White);

					renderer->Draw(mpText);
                    UIUtils::DrawNineSlice(&renderer.value(), textures["BaseBar"], baseMPBar, 20.0f, sf::Color::White);
					UIUtils::DrawNineSlice(&renderer.value(), textures["MPBar"], currentMPBar, 20.0f, sf::Color::White);

					renderer->Draw(levelText);
					renderer->Draw(expText);
					UIUtils::DrawNineSlice(&renderer.value(), textures["BaseBar"], baseEXPBar, 20.0f, sf::Color::White);
					UIUtils::DrawNineSlice(&renderer.value(), textures["EXPBar"], currentEXPBar, 20.0f, sf::Color::White);
                }
                else {
                    sf::Text detailText(fonts["MainFont"], sf::String::fromUtf8(menuDetails[i].description.begin(), menuDetails[i].description.end()), 24);
                    detailText.setFillColor(sf::Color(50, 50, 50));
                    detailText.setStyle(sf::Text::Regular);
                    detailText.setPosition({ startDetailX, startDetailY });
                    renderer->Draw(detailText);
                }
            }
		}
    }

    renderer->Display();
}