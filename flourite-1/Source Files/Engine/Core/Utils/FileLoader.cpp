// FileLoader.cpp

#include "Engine/Core/Utils/FileLoader.h"
#include <fstream>
#include <iostream>

namespace FileLoader {
    void LoadFile(const std::string& filepath, std::vector<std::string>& outLines) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[FileLoader Error] Failed to open file: " << filepath << std::endl;
            return;
        }

        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) {
            std::cerr << "[FileLoader Error] No file extension found in: " << filepath << std::endl;
            return;
        }


        std::string line;
        while (std::getline(file, line)) {
            outLines.push_back(line);
            // std::cout << line << std::endl; // 너무 많은 I/O 호출 방지를 위해 평소엔 주석 처리 권장
        }

        file.close();
    }
}