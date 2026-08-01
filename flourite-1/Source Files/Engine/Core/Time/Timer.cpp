// Timer.cpp

#include "Engine/Core/Time/Timer.h"

// 나노 초(ns) 표현
using namespace std::chrono_literals;

Timer::Timer() : deltaTime(0ns) {
	// 최종 시간을 현재로 기록 맟 DeltaTime 초기화
	lastTime = std::chrono::steady_clock::now();
}

// 프레임 시간을 재고 accumulator에 누적
void Timer::Tick() {
	auto currentTime = std::chrono::steady_clock::now();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
}

// DeltaTime 반환
std::chrono::nanoseconds Timer::GetDeltaTime() const {
	return deltaTime;
}

// DeltaTime을 초 단위로 반환
double Timer::GetDeltaTimeSeconds() const {
	return std::chrono::duration<double>(deltaTime).count();
}
