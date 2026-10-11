// PerformanceTracker.cpp

#include "Engine/Core/Utils/PerformanceTracker.h"
#include <iomanip>
#include <fstream>
#include <ctime>
#include <iostream>
#include <numeric>   // std::accumulate
#include <algorithm> // std::sort

// cpp에서는 기본 인수를 생략하고 구현만 작성합니다.
PerformanceTracker::PerformanceTracker(size_t sampleSize) : maxSamples(sampleSize) {
    deltaTimes.reserve(maxSamples);
}

void PerformanceTracker::RecordFrame(std::chrono::nanoseconds nsDT, int allocs, int draws, int swaps, int activated) {
    if (deltaTimes.size() < maxSamples) {
        float seconds = std::chrono::duration<float>(nsDT).count();
        deltaTimes.push_back(seconds);

        // 1. 할당
        if (allocs > maxAllocationsInFrame) maxAllocationsInFrame = allocs;
        if (allocs < minAllocationsInFrame) minAllocationsInFrame = allocs;
        sumAllocations += allocs;

        // 2. 드로우 콜
        if (draws > maxDrawCallsInFrame) maxDrawCallsInFrame = draws;
        if (draws < minDrawCallsInFrame) minDrawCallsInFrame = draws;
        sumDrawCalls += draws;

        // 3. 텍스처 스왑
        if (swaps > maxTextureSwapsInFrame) maxTextureSwapsInFrame = swaps;
        if (swaps < minTextureSwapsInFrame) minTextureSwapsInFrame = swaps;
        sumTextureSwaps += swaps;

        // 4. 물리 활성화 객체 수
        if (activated > maxActivateCount) maxActivateCount = activated;
        sumActivateCount += activated;

    }
}

// Fixed 관련 정보 업데이트는 따로(해당 정보를 알 수 있는 위치가 너무 깊음)
// RecordFrame과 한 반복에 1:1로 불리고 PrintStatistics가 즉시 뒤따르므로 분모는 deltaTimes.size()와 같음
void PerformanceTracker::RecordFixedStepLoop(std::chrono::nanoseconds ns, int count) {
    // 시간
    float time = std::chrono::duration<float>(ns).count();
    if (time > maxFixedStepLoopTime) maxFixedStepLoopTime = time;
    sumFixedStepLoopTime += time;

    // 카운트
    if (count > maxFixedStepCount) maxFixedStepCount = count;
    sumFixedStepCount += count;
}

bool PerformanceTracker::IsSampleFull() const {
    return deltaTimes.size() >= maxSamples;
}

void PerformanceTracker::PrintStatistics() {
    if (deltaTimes.empty()) return;

    std::sort(deltaTimes.begin(), deltaTimes.end());

    float minDT = deltaTimes.front();
    float maxDT = deltaTimes.back();
    float sum = std::accumulate(deltaTimes.begin(), deltaTimes.end(), 0.0);
    float avgDT = sum / deltaTimes.size();
    float medianDT = deltaTimes[deltaTimes.size() / 2];

    float avgAllocs = static_cast<float>(sumAllocations) / deltaTimes.size();
    float avgDraws = static_cast<float>(sumDrawCalls) / deltaTimes.size();
    float avgSwaps = static_cast<float>(sumTextureSwaps) / deltaTimes.size();
    float avgActivateCount = static_cast<float>(sumActivateCount) / deltaTimes.size();

    float avgFixedStepLoopTime = static_cast<float>(sumFixedStepLoopTime) / deltaTimes.size();
    float avgFixedStepCount = static_cast<float>(sumFixedStepCount) / deltaTimes.size();

    // CSV 파일로 데이터 내보내기
    std::string filename = "benchmark_result.csv";
    std::ifstream infile(filename);
    bool fileExists = infile.good();
    infile.close();

    std::ofstream outfile(filename, std::ios::app);

    if (outfile.is_open()) {
        if (!fileExists) {
            outfile << "Date,Samples,MinDT,MaxDT,AvgDT,MedianDT,"
                << "MaxAlloc,AvgAlloc,MaxDrawCalls,AvgDrawCalls,MaxSwaps,AvgSwaps,MaxActivateCount,AvgActivateCount,MaxFixedStepLoopTime,AvgFixedStepLoopTime,MaxFixedStepCount,AvgFixedStepCount\n";
        }

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now_time);
        std::string timeString(timeStr);
        timeString.pop_back();

        outfile << std::fixed << std::setprecision(6)
            << timeString << ","
            << deltaTimes.size() << ","
            << minDT << "," << maxDT << "," << avgDT << "," << medianDT << ","
            << maxAllocationsInFrame << "," << avgAllocs << ","
            << maxDrawCallsInFrame << "," << avgDraws << ","
            << maxTextureSwapsInFrame << "," << avgSwaps << "," << maxActivateCount << "," << avgActivateCount << "," << maxFixedStepLoopTime << "," << avgFixedStepLoopTime << "," << maxFixedStepCount << "," << avgFixedStepCount << "\n";

        outfile.close();

        csvRecordCount++;
        std::cout << ">> [" << csvRecordCount << "]" << filename << " Updated << \n";
    }
    else {
        std::cout << "[Warning] benchmark_result.csv 파일을 열 수 없습니다.\n";
    }

    // 측정마다 모든 변수 리셋
    deltaTimes.clear();
    maxAllocationsInFrame = 0;
    minAllocationsInFrame = std::numeric_limits<int>::max();
    sumAllocations = 0;

    maxDrawCallsInFrame = 0;
    minDrawCallsInFrame = std::numeric_limits<int>::max();
    sumDrawCalls = 0;

    maxTextureSwapsInFrame = 0;
    minTextureSwapsInFrame = std::numeric_limits<int>::max();
    sumTextureSwaps = 0;

    maxActivateCount = 0;
    sumActivateCount = 0;

    maxFixedStepLoopTime = 0;
    sumFixedStepLoopTime = 0;
    maxFixedStepCount = 0;
    sumFixedStepCount = 0;
}