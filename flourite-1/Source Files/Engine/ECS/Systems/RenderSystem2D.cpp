#include "Engine/ECS/Systems/RenderSystem2D.h"
#include <algorithm> // std::sort 사용을 위함

RenderSystem2D::RenderSystem2D(Renderer& r) : renderer(r) {
}

void RenderSystem2D::Render(EntityManager& registry, float alpha) {
    // 1. 매 프레임 벡터를 새로 만들지 않고, 기존 벡터의 내용물만 비웁니다 (할당 발생 X)
    renderables.clear();

    // (선택 사항) 만약 엔티티 개수가 벡터의 수용량보다 많아졌을 때만 늘려줍니다.
    if (renderables.capacity() < registry.signatures.size()) {
        renderables.reserve(registry.signatures.size());
    }

    // 인덱스 값 업데이트
    maxIndex = registry.GetEntityCapacity();

    // 카메라 시야 경계 계산
    sf::Vector2f center = renderer.GetView().getCenter();
    sf::Vector2f size = renderer.GetView().getSize();

    // 화면 끝자락에서 갑자기 생성/사라짐 방지용 패딩
    float padding = 150.f;

    float camLeft = center.x - (size.x / 2.f) - padding;
    float camRight = center.x + (size.x / 2.f) + padding;
	float camTop = center.y - (size.y / 2.f) - padding;
	float camBottom = center.y + (size.y / 2.f) + padding;

    // 렌더 데이터 수집 시 시야 밖의 엔티티는 컬링
    for (Entity i = 0; i < maxIndex;  ++i) {
        if (!registry.signatures[i].test(COMP_TRANSFORM) || !registry.signatures[i].test(COMP_RENDER)) continue;

        auto& trans = registry.transforms[i];
		auto& rend = registry.renders[i];

        // 보간된 현재 화면 렌더링 좌표
        float renderX = trans.previousPosition.x + (trans.position.x - trans.previousPosition.x) * alpha;
        float renderY = trans.previousPosition.y + (trans.position.y - trans.previousPosition.y) * alpha;
        
        // 프러스텀 컬링 로직
        if (renderX < camLeft || renderX > camRight || renderY < camTop || renderY > camBottom) {
            // 시야 밖이므로 렌더 대상에서 제외, 다음 엔티티로
            continue;
        }

        if (registry.signatures[i].test(COMP_TRANSFORM) && registry.signatures[i].test(COMP_RENDER)) {
            renderables.push_back(i);
        }
		
    }

    // 2. Layer 값을 기준으로 오름차순 정렬
    std::sort(renderables.begin(), renderables.end(), [&](Entity a, Entity b) {
        const auto& rA = registry.renders[a];
        const auto& rB = registry.renders[b];

        // 1순위: 레이어 (Layer 1 -> Layer 2 순서로 그리기)
        if (rA.layer != rB.layer) {
            return rA.layer < rB.layer;
        }

        // 2순위: 레이어가 같다면 Y좌표 정렬 (아래에 있을수록 앞에 그림)
        const auto& tA = registry.transforms[a];
        const auto& tB = registry.transforms[b];
        return tA.position.y < tB.position.y;
        });



    // 3. 정렬된 순서대로 그리기 (이하 동일)
    for (Entity e : renderables) {
        auto& trans = registry.transforms[e];
        auto& rend = registry.renders[e];

        if (!rend.texture) continue; // 텍스처가 없으면 그리지 않음

        float renderX = trans.previousPosition.x * (1.0f - alpha) + trans.position.x * alpha;
        float renderY = trans.previousPosition.y * (1.0f - alpha) + trans.position.y * alpha;

        // [SFML 3 대응] ECS 데이터로 매 프레임 가벼운 껍데기(Sprite)를 만들어 그립니다.
        sf::Sprite sprite(*rend.texture);

        // 에러 해결: setPosition(x, y) 금지 -> setPosition(sf::Vector2f(x, y)) 강제 적용
        sprite.setPosition(sf::Vector2f(static_cast<float>(renderX), static_cast<float>(renderY)));
        sprite.setScale(sf::Vector2f(static_cast<float>(trans.scale.x), static_cast<float>(trans.scale.y)));

        // 데이터 적용
        sprite.setOrigin(rend.origin);
        sprite.setColor(rend.color);
        if (rend.textureRect.size != sf::Vector2i(0, 0)) {
            sprite.setTextureRect(rend.textureRect);
        }

        renderer.Draw(sprite, rend.texture);
    }
}