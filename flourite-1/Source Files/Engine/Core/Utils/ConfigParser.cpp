// ConfigParser.cpp

#include "Engine/Core/Utils/ConfigParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include <vector>
#include <iostream>

namespace ConfigParser {
    std::unordered_map<std::string, std::string> settings;

    // 단일 INI 파일 로드: 기존 settings에 누적(덮어쓰기)
    void LoadConfig(const std::string& filepath) {
        std::vector<std::string> lines;
        FileLoader::LoadFile(filepath, lines);

        std::string currentSection = "";
        for (const std::string& rawLine : lines) {
            std::string line = StringUtils::Trim(rawLine);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2) + ".";
                continue;
            }

            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = currentSection + StringUtils::Trim(line.substr(0, equalPos));
                std::string value = StringUtils::Trim(line.substr(equalPos + 1));
                settings[key] = value;
            }
        }
        std::cout << "[ConfigParser] 로드 완료: " << filepath << "\n";
    }

    int GetInt(const std::string& key, int defaultValue) {
        auto it = settings.find(key);
        if (it != settings.end()) {
            try { return std::stoi(it->second); }
            catch (...) {
                std::cout << "[Config Error] '" << key << "'의 값이 숫자가 아닙니다: " << it->second << "\n";
                return defaultValue;
            }
        }
        return defaultValue;
    }

    float GetFloat(const std::string& key, float defaultValue) {
        auto it = settings.find(key);
        return (it != settings.end()) ? std::stof(it->second) : defaultValue;
    }

    std::string GetString(const std::string& key, const std::string& defaultValue) {
        auto it = settings.find(key);
        return (it != settings.end()) ? it->second : defaultValue;
    }

    // 콤마로 구분된 문자열을 vector로 반환
    // 예: CharacterTypes = Player,player,NPC,npc → {"Player","player","NPC","npc"}
    std::vector<std::string> GetStringList(const std::string& key, const std::string& defaultValue) {
        std::string raw = GetString(key, defaultValue);
        std::vector<std::string> result;
        std::stringstream ss(raw);
        std::string token;
        while (std::getline(ss, token, ',')) {
            std::string trimmed = StringUtils::Trim(token);
            if (!trimmed.empty()) result.push_back(trimmed);
        }
        return result;
    }
}