// 메뉴 초안
#include "Engine/Modules/Event/EventHandler.h"

enum class GameState {
	PLAYING,      // 평소 이동 상태
	DIALOG,       // 대화 중 (물리 정지, UI 활성화)
	PAUSE_MENU    // 메뉴 창 열림 (물리 정지, 메뉴 UI 활성화)
};

class Menu {
private:
	// ! 신규: 일단 테스트를 위해 만들어 둠. 실제 파일 분리 시 얘는 지워야 함. 이에 따라 bool MenuOpened는 일단 삭제
	GameState currentState = GameState::PLAYING;
	Event::EventHandler eh;
private:
	

public:
	Menu() {}
	~Menu() {}
	
	// 해당 클래스 외부에서 접근 가능한 함수는 최소한으로 제한할 것임

	// 만약 실행 중 이벤트 핸들러에서 "Menu"에 해당하는 키를 입력받았다면 해당 함수가 실행되도록 다른 파일에서도 처리 필요함

	void OpenMenu() {
		// eh에서 ESC 누르면 
		// 일단 state부터 업데이트
		

		// 시간 배율을 0배로 설정해야 함. FixedStepAccumulator에 SetTimeScale(float scale)등을 추가할 필요가 있음. 시간인 만큼 부동소수점을 고려하여 double 또는 그냥 int 형태로 배수 적용까지 고려중.
		// 아직 없는 함수임

		// 레이어 맨 위에 창 표시. 이거는 렌더 시스템에 있을 거 같음
		// config 파일에 입력된 경로에 따라 이미지(들) 로드
		// 레이어 맨 위에 표시

		// 메뉴가 열린 상태일 때 조작 처리는 내부 다른 함수로 분리하면 될 것으로 보임. void MenuControl() 실행하기
		MenuControl();
	}

	void MenuControl() {
		// Move_Up 등 이동 관련 키 입력을 해도 시간이 0배이기 때문에 안 움직일 것임. 그럼에도 비활성화 해주는 것이 안전할 것으로 보임.
		// 물리 비활성화 로직

		// Menu 관련 키만 작동하도록 변경 필요.

		// "Menu" 키 입력 시 메뉴 닫기 함수 실행하기
	}

	void closeMenu() {
		// 변경한 항목들 전부 다시 돌려놓기

		// 최후에 상태 업데이트
	}
	
};