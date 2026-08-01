// DialogParser.h

#pragma once
#include <string>
#include <unordered_map>
#include "DialogData.h"

namespace DialogParser {
    // Dialog는 전역 상태(DialogContext)에 다이렉트로 꽂아 넣으므로 파일 경로 하나만 받습니다.
    void ParseDialogConfig(const std::string& filepath);
}