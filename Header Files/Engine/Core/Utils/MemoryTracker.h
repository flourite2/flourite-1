// MemoryTracker.h

#pragma once
#include <cstddef>

// 전역 변수로 선언하여 메인 루프 및 PerformanceTracker에서 접근 가능하도록 함
extern bool g_trackingEnabled;
extern int  g_allocCountPerFrame;