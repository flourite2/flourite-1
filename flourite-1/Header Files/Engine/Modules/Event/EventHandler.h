// EventHandler.h

#pragma once
#include "Engine/Modules/Render/Renderer.h"
#include "Engine/Modules/Input/InputManager.h"

namespace Event {
    class EventHandler {
    public:
        // 상태를 가질 필요가 없으므로 정적(static) 함수로 선언하는 것이 깔끔합니다.
        static void ProcessEvents(Renderer& renderer, const InputManager& inputManager);
    };
}