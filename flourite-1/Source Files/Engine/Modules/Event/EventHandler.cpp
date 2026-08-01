// EventHandler.cpp

#include "Engine/Modules/Event/EventHandler.h"

namespace Event {
    // InputManager는 handleEvent가 const 메서드이므로 const 참조로 받습니다.
    void EventHandler::ProcessEvents(Renderer& renderer, const InputManager& inputManager) {
        while (const auto event = renderer.PollEvent()) {

            // 시스템 공통 이벤트 처리: 창 닫기
            if (event->is<sf::Event::Closed>()) {
                renderer.Close();
            }

            // 입력 상호작용 트리거 이벤트 처리
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // 키 대조 없이 입력 매니저의 실행 사전으로 바로 실행
                inputManager.handleEvent(keyPressed->code);
            }
        }
    }
}