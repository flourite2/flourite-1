#pragma once

#include <iostream>
#include <Engine/ECS/Systems/RenderSystem2D.h>
#include <Engine/Modules/UI/UIUtils.h>
#include <string>

// 메뉴 초안. 상태 필요 없을 거 같으니까 그냥 struct 또는 namespace로 선언할 생각임
namespace Menu {
	void OpenMenu() {
		// 레이어 맨 위에 창 표시. 이거는 렌더 시스템에 있을 거 같음
		// config 파일에 입력된 경로에 따라 이미지(들) 렌더 (임시)
		std::cout << "메뉴 창 열림(임시)" << std::endl;
		std::cout << "레이어는 가장 위로 지정해야 함." << std::endl;


	}

	void CloseMenu() {
		std::cout << "메뉴 창 닫음(임시)" << std::endl;
	}

};