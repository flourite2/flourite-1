// DialogData.h

#pragma once
#include <string>
#include <vector>

// 가상 함수, 프라이빗 제한이 전혀 없는 순수 가벼운 데이터 구조체 (POD)
struct DialogData {
    int dialogID = -1;
    std::string speakerID;
    std::string text;
    int nextDialogID = -1;

    // ★ 다중 선택지 배열 추가: <선택지 화면 텍스트, 분기될 다음 대화 ID>
    std::vector<std::pair<std::string, int>> choices;
};

// 로딩 타임에 딱 한 번만 빌드할 정적 데이터 레지스트리
namespace DialogContext {
    inline std::vector<DialogData> staticTable;

    // ID 정렬 및 고속 이진 탐색 기법 적용
    inline const DialogData* Find(int id) {
        auto it = std::lower_bound(staticTable.begin(), staticTable.end(), id,
            [](const DialogData& data, int targetID) { return data.dialogID < targetID; });

        if (it != staticTable.end() && it->dialogID == id) {
            return &(*it); // 포인터 직접 반환으로 복사 오버헤드 제로
        }
        return nullptr;
    }
}