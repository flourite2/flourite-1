// DialogParser.cpp

#include "Engine/Modules/Dialog/DialogParser.h"
#include "Engine/Modules/Dialog/DialogData.h"
#include "Engine/Core/Utils/StringUtils.h"
#include "Engine/Core/Utils/FileLoader.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace DialogParser {
    void ParseDialogConfig(const std::string& filepath) {
        std::vector<std::string> outLines;
        std::string extension;

        // 1. 안전한 파일 로드
        FileLoader::LoadFile(filepath, outLines, extension);

        if (outLines.empty()) {
            std::cerr << "[DialogParser Error] 파일을 읽을 수 없거나 내용이 텅 비어있습니다: " << filepath << "\n";
            return;
        }

        // 기존 메모리 싹 비우기
        DialogContext::staticTable.clear();

        bool headerSkipped = false;
        for (const std::string& rawLine : outLines) {
            std::string checkLine = StringUtils::Trim(rawLine);

            // 첫 줄(헤더) 무조건 스킵
            if (!headerSkipped) {
                headerSkipped = true;
                continue;
            }

            if (checkLine.empty() || checkLine[0] == '#') continue;

            

            // 하드코딩 제거된 유연한 파싱
            std::stringstream ss(rawLine);
            std::vector<std::string> cols;
            std::string token;

            while (std::getline(ss, token, '\t')) {
                cols.push_back(token);
            }

            if (cols.size() < 4) {
                std::cerr << "[DialogParser Warning] 컬럼 부족: " << rawLine << "\n";
                continue;
            }

            // ★ 여기서부터 try 블록 시작!
            try {
                DialogData data;
                data.dialogID = std::stoi(StringUtils::Trim(cols[0]));
                data.speakerID = StringUtils::Trim(cols[1]);

                std::string text = StringUtils::Trim(cols[2]);
                if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
                    text = text.substr(1, text.size() - 2);
                }
                data.text = text;

                data.nextDialogID = std::stoi(StringUtils::Trim(cols[3]));

                // 5번째 컬럼(Choices) 파싱

                /*if (cols.size() >= 5) {
                    std::string rawChoices = StringUtils::Trim(cols[4]);
                    if (!rawChoices.empty() && rawChoices != "-") {
                        std::stringstream choiceStream(rawChoices);
                        std::string choiceToken;
                        while (std::getline(choiceStream, choiceToken, '|')) {
                            size_t colonPos = choiceToken.find(':');
                            if (colonPos != std::string::npos) {
                                data.choices.push_back({
                                    choiceToken.substr(0, colonPos),
                                    std::stoi(choiceToken.substr(colonPos + 1))
                                    });
                            }
                        }
                    }
                }*/

                if (cols.size() >= 5) {
                    if (!cols[4].empty() && cols[4] != "-") {
                        std::vector<std::string>choices = StringUtils::Split(cols[4], '|');
                        for (int i = 0; i < choices.size(); i++) {
                            std::vector<std::string> choiceTokens = StringUtils::Split(choices[i], ':');
                            if (choiceTokens.size() == 2) {
                                data.choices.push_back({
                                        choiceTokens[0],
                                        std::stoi(choiceTokens[1])
                                    });
                            }
                            else {
                                std::cerr << "[DialogParser Error] choiceTokens.size()가 2가 아님: " << choiceTokens[0] << choiceTokens[1] << std::endl;
                            }
                        }
                    }
                }

                DialogContext::staticTable.push_back(data);
            } // ★ try 블록 끝
            catch (const std::exception& e) {
                std::cerr << "[DialogParser Error] ID 숫자 변환 실패 (데이터에 문자가 섞여있는지 확인): " << rawLine << "\n";
            }
        }

        // 5. 이진 탐색을 위한 ID 오름차순 정렬
        std::sort(DialogContext::staticTable.begin(), DialogContext::staticTable.end(),
            [](const DialogData& a, const DialogData& b) { return a.dialogID < b.dialogID; });

        std::cout << "[System] Dialog 파싱 완료. 현재 적재된 대사 수: " << DialogContext::staticTable.size() << "개\n";
    }
}