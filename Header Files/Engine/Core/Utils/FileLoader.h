// FileLoader.h

#pragma once
#include <string>
#include <vector>

namespace FileLoader {
    // 파일 경로 업데이트 및 불러오기
    void LoadFile(const std::string& filepath, std::vector<std::string>& outLines);
}