// DialogStatus.h

#pragma once
#include "DialogData.h"
#include <SFML/Graphics.hpp>
#include <iostream>

struct DialogStatus {
    const DialogData* currentDialog = nullptr;
    bool isSpeaking = false;
    bool isWaitingForChoice = false;
    int currentChoiceIndex = 0;

    sf::String wrappedText;
    float typingTimer = 0.0f;
    size_t visibleCharCount = 0;
    bool isTypingFinished = true;
    float characterSpeed = 0.03f;

    inline void Start(int id, const sf::Font& font, unsigned int charSize, float maxWidth) {
        currentDialog = DialogContext::Find(id);

        if (!currentDialog) {
            std::cout << "[Dialog Error] ID " << id << "번 대사를 찾을 수 없습니다!\n";
            isSpeaking = false;
            return;
        }

        isSpeaking = true;
        currentChoiceIndex = 0;
        isWaitingForChoice = false;

        typingTimer = 0.0f;
        visibleCharCount = 0;
        isTypingFinished = false;

        wrappedText = ApplyWordWrap(currentDialog->text, maxWidth, font, charSize);
    }

    inline void Update(float dt) {
        if (!isSpeaking || isTypingFinished) return;

        typingTimer += dt;
        if (typingTimer >= characterSpeed) {
            typingTimer = 0.0f;
            visibleCharCount++;

            if (visibleCharCount >= wrappedText.getSize()) {
                visibleCharCount = wrappedText.getSize();
                isTypingFinished = true;

                if (!currentDialog->choices.empty()) {
                    isWaitingForChoice = true;
                }
            }
        }
    }

    inline void Skip() {
        if (!isSpeaking || isTypingFinished) return;
        visibleCharCount = wrappedText.getSize();
        isTypingFinished = true;

        if (!currentDialog->choices.empty()) {
            isWaitingForChoice = true;
        }
    }

private:
    inline sf::String ApplyWordWrap(const std::string& str, float maxWidth, const sf::Font& font, unsigned int charSize) {
        sf::String result;
        sf::String currentLine;
        sf::String sfStr = sf::String::fromUtf8(str.begin(), str.end());
        sf::Text text(font, "", charSize);

        for (size_t i = 0; i < sfStr.getSize(); ++i) {
            // ★ 수정 1: uint32_t 대신 명확한 유니코드 타입인 char32_t 사용
            char32_t glyph = sfStr[i];

            // ★ 수정 2: 일반 문자열과의 덧셈을 분리하고, UTF-32 리터럴(U) 사용
            if (glyph == U'\n' || glyph == '\n') {
                result += currentLine;
                result += U"\n";
                currentLine = U"";
                continue;
            }

            currentLine += glyph;
            text.setString(currentLine);

            if (text.getLocalBounds().size.x > maxWidth) {
                if (currentLine.getSize() > 1) {
                    result += currentLine.substring(0, currentLine.getSize() - 1);
                    result += U"\n";
                    currentLine = sf::String(glyph); // 이제 모호성 에러 발생 안 함!
                }
                else {
                    result += currentLine;
                    result += U"\n";
                    currentLine = U"";
                }
            }
        }
        result += currentLine;
        return result;
    }
};

inline DialogStatus g_DialogStatus;