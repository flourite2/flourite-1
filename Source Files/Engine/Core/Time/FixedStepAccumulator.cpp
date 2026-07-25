// FixedStepAccumulator.cpp

#include "Engine/Core/Time/FixedStepAccumulator.h"

// 나노 초(ns) 표현
using namespace std::chrono_literals;

FixedStepAccumulator::FixedStepAccumulator(int hz) : 
	timeScale(1.0), accumulator(0ns) {
	// 실수 레벨 목표 dt를 nanoseconds로 cast하여 targetDT에 저장
	targetDT = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / hz));
}

// 이 로직은 프레임마다 수백 회 실행될 수 있기 때문에 예외를 던지지 않도록 함
void FixedStepAccumulator::Update(std::chrono::nanoseconds rawDT) noexcept {
	// 실제 흐른 시간 저장 및 배율 적용
	
	// ns에 배율 곱하기
	// 정수형 accumulator에 더하기
	accumulator += std::chrono::duration_cast<std::chrono::nanoseconds>(
		rawDT * timeScale
	);
}

int FixedStepAccumulator::GetRepeatCount() {
	int maxCount = 5;
	int repeatCount = 0;

	// 저장된 지연 시간이 너무 길다면 max count까지만 실행 가능한 만큼만 accumulator에 저장
	if (accumulator > targetDT * maxCount) {
		accumulator = targetDT * maxCount;
	}

	// 최대 5번만 반복
	while (repeatCount < maxCount && accumulator >= targetDT) {
		accumulator -= targetDT;
		repeatCount++;
	}

	return repeatCount;
}

// Alpha 값 반환
double FixedStepAccumulator::GetAlpha() const {
	// 실수 형태로 전환
	return std::chrono::duration<double>(accumulator).count() / std::chrono::duration<double>(targetDT).count();
}