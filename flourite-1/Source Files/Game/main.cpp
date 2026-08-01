// main.cpp

#include "Engine/Application/Application.h"
#include <iostream>

int main() {
    try {
        Application app;

        // 2단계 초기화: 엔진 초기화가 성공적으로 끝났을 때만 루프를 실행
        if (app.Init()) {
            app.Run();
        }
        else {
            std::cerr << "엔진 초기화에 실패하여 게임을 종료합니다.\n";
            return -1;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Fatal Error: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}