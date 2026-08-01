// SpawnSystem.h
#pragma once
#include <unordered_map>
#include <string>
#include "Engine/ECS/Entities/EntityManager.h"
#include "Engine/Modules/Being/BeingParser.h"
#include "Engine/Modules/Render/RenderParser.h"

// 텍스처 공급자: 인터페이스 AnimationManager 또는 ResourceManager가 구현합니다.
// SpawnSystem이 텍스처 소유권에 직접 결합되지 않도록 분리합니다.
class ITextureProvider {
public:
    virtual const sf::Texture* GetTexture(const std::string& key) = 0;
    virtual ~ITextureProvider() = default;
};

class SpawnSystem {
public:
    // beingTable  : BeingParser가 파싱한 청사진 사전
    // spawnPoints : RenderParser::LoadMapJSON 이 채운 스폰 포인트 목록
    // registry    : ECS 엔티티 관리자
    // textures    : 텍스처 공급자 (AnimationManager 등)
    static void SpawnAll(
        const std::unordered_map<std::string, BeingMapData>& beingTable,
        const std::vector<SpawnPointData>& spawnPoints,
        EntityManager& registry,
        ITextureProvider& textures
    );

private:
    // AIType 문자열을 열거형으로 변환
    static AIType ParseAIType(const std::string& str);

    // 타입별 조립 함수: 공통 컴포넌트 세팅 후 호출됩니다.
    static void AssemblePlayer(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);

    static void AssembleNPC(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);

    static void AssemblePropDynamic(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);

    static void AssemblePropStatic(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);

    static void AssemblePropOverhead(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);

    static void AssembleDummy(Entity e, const BeingMapData& data,
        EntityManager& registry, ITextureProvider& textures);
};