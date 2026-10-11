// PerformanceTracker.h

#pragma once
#include <vector>
#include <chrono>
#include <limits> // 최댓값 사용을 위해 추가
#include <string>

class PerformanceTracker {
private:
    size_t maxSamples;
    std::vector<float> deltaTimes;

    // 할당 추적
    int maxAllocationsInFrame = 0;
    int minAllocationsInFrame = std::numeric_limits<int>::max();
    long long sumAllocations = 0;

    // 드로우 콜 추적
    int maxDrawCallsInFrame = 0;
    int minDrawCallsInFrame = std::numeric_limits<int>::max();
    long long sumDrawCalls = 0;

    // 텍스처 스왑 추적
    int maxTextureSwapsInFrame = 0;
    int minTextureSwapsInFrame = std::numeric_limits<int>::max();
    long long sumTextureSwaps = 0;

    // 현재 물리 활성화된 엔티티 수를 기록하기
    int maxActivateCount = 0;
    long long sumActivateCount = 0;

    // Fixed 스텝 관련 정보
    float maxFixedStepLoopTime = 0;
    double sumFixedStepLoopTime = 0;
    int maxFixedStepCount = 0;
    long long sumFixedStepCount = 0;

    // csv 파일에 기록한 횟수(몇 번째 기록인지)
    int csvRecordCount = 0;

public:
    // 기본 인수는 반드시 헤더에 명시해야 합니다.
    PerformanceTracker(size_t sampleSize = 1000);

    void RecordFrame(std::chrono::nanoseconds nsDT, int allocs, int draws, int swaps, int activated);
    void RecordFixedStepLoop(std::chrono::nanoseconds ns, int count);
    bool IsSampleFull() const;
    void PrintStatistics();
};