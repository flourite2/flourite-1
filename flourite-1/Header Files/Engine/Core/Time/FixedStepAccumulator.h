// FixedStepAccumulator.h

#pragma once
#include <chrono>

class FixedStepAccumulator {
private:
	double timeScale;
	std::chrono::nanoseconds accumulator;
	std::chrono::nanoseconds targetDT;

public:
	// 생성 당시에 목표 주기 설정
	FixedStepAccumulator(int hz);

	// 매 루프 시작 시 Timer의 deltaTime(rawDT)를 받아 timeScale과 곱하여 accumulator에 누적
	void Update(std::chrono::nanoseconds rawDT) noexcept;

	// while(accumulator >= targetDT){repeatCount++; accumulator - targetDT;} -> return repeatCount;
	int GetRepeatCount();

	// return (double)accumulator.count() / (double)targetDT.count();
	double GetAlpha() const;

};