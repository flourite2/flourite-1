// InputParser.cpp

#include "Engine/Modules/Input/InputParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include <iostream>
#include <sstream>

namespace InputParser {
    void InputParser::ParseInputConfig(const std::string& filepath, std::unordered_map<std::string, InputMapData>& inputTable) {
        std::vector<std::string> outLines;
        FileLoader::LoadFile(filepath, outLines);
        for (const std::string& rawLine : outLines) {
            std::string line = StringUtils::Trim(rawLine);
            if (line.empty() || line[0] == ';' || line[0] == '[') continue;

            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string rawAction = line.substr(0, equalPos);
                std::string rawInput = line.substr(equalPos + 1);

                std::string action = StringUtils::Trim(rawAction);
                std::string input = StringUtils::Trim(rawInput);

                inputTable[action] = InputMapData{ action, input };
                // std::cout << "[ini Debug] Loaded Input Mapping: Action='" << action << "', Input='" << input << "'" << std::endl;
            }
        }
    }
    
}