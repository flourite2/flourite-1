// MemoryTracker.cpp

#include "Engine/Core/Utils/MemoryTracker.h"
#include <cstdlib>
#include <new>

bool g_trackingEnabled = false;
int  g_allocCountPerFrame = 0;

// 글로벌 new 연산자 오버로딩 (구현부는 헤더가 아닌 cpp에만 존재해야 중복 정의 에러가 안 납니다)
#pragma warning(disable: 28251)
void* operator new(size_t size) {
    if (g_trackingEnabled)
        g_allocCountPerFrame++;
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

void operator delete(void* memory, size_t size) noexcept {
    free(memory);
}