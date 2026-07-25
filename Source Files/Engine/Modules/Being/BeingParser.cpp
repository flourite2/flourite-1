// BeingParser.cpp
#include "Engine/Modules/Being/BeingParser.h"
#include "Engine/Core/Utils/FileLoader.h"
#include "Engine/Core/Utils/StringUtils.h"
#include <sstream>
#include <iostream>

namespace BeingParser {

    void ParseBeingConfig(const std::string& filepath,
        std::unordered_map<std::string, BeingMapData>& beingTable)
    {
        std::vector<std::string> outLines;
        std::string extension;
        FileLoader::LoadFile(filepath, outLines, extension);

        // 컬럼 인덱스 (헤더 순서: ID Type AIType MapID PosX PosY Mass Radius TextureKey DialogID)
        // #ID  Type  AIType  MapID  PosX  PosY  Mass  Radius  TextureKey  DialogID
        //  0    1      2      3      4     5     6      7         8          9

        bool headerSkipped = false;
        for (const std::string& rawLine : outLines) {
            std::string checkLine = StringUtils::Trim(rawLine);
            if (checkLine.empty() || checkLine[0] == '#') continue;

            // 첫 번째 유효 줄은 헤더
            if (!headerSkipped) {
                headerSkipped = true;
                continue;
            }

            // ★ 하드코딩된 배열(cols[11]) 제거!
            std::stringstream ss(rawLine);
            std::vector<std::string> cols;
            std::string token;

            while (std::getline(ss, token, '\t')) {
                cols.push_back(token);
            }

            // 구버전 TSV(컬럼 10개)와 신버전(컬럼 11개)을 모두 포용할 수 있도록 유연하게 설계
            if (cols.size() < 10) {
                std::cerr << "[BeingParser] 필수 컬럼 부족, 건너뜀: " << rawLine << "\n";
                continue;
            }

            BeingMapData data;
            data.id = StringUtils::Trim(cols[0]);
            data.type = StringUtils::Trim(cols[1]);
            data.aiType = StringUtils::Trim(cols[2]);
            data.mapID = StringUtils::Trim(cols[3]);
            data.posX = std::stof(cols[4]);
            data.posY = std::stof(cols[5]);
            data.mass = std::stof(cols[6]);
            data.radius = std::stof(cols[7]);
            data.textureKey = StringUtils::Trim(cols[8]);
            data.dialogID = std::stoi(cols[9]);

            // 11번째 컬럼(UIName)이 있으면 넣고, 없으면 기본 ID를 UIName으로 사용(Fallback)
            if (cols.size() >= 11) {
                data.uiName = StringUtils::Trim(cols[10]);
            }
            else {
                data.uiName = data.id;
            }

            beingTable[data.id] = data;

            std::cout << "[BeingParser] 로드: id=" << data.id
                << " type=" << data.type
                << " aiType=" << data.aiType
                << " dialogID=" << data.dialogID << "\n";
        }
    }
}