// Timer.h

#pragma once
#include <chrono>

class Timer {
private:
	// 마지막 시간 객체 및 실제 DeltaTime
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::nanoseconds deltaTime;

public:
	Timer();							// 생성자
	void Tick();						// 프레임 갱신

	// 초정밀 시간 단위로 DeltaTime 반환
	std::chrono::nanoseconds GetDeltaTime() const;			// dt 반환
	double GetDeltaTimeSeconds() const;	// dt를 초 단위로 반환(타 시스템 연계 목적)
};