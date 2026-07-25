// AnimationHelper.h

#include "Engine/Modules/Render/AnimationHelper.h"
#include <iostream>

namespace AnimationHelper {
    std::unordered_map<std::string, sf::Texture> textureCache;
    const sf::Texture* animLookupTable[4][3];
    std::string keyLookupTable[4][3];

    void Initialize(const std::unordered_map<std::string, RenderMapData>& renderTable, const std::string& basePath) {
        for (const auto& [key, data] : renderTable) {
            if (data.category == "character") {
                if (!textureCache[key].loadFromFile(basePath + data.filename)) {
                    std::cout << "[Critical VRAM Error] Fail to load: " << data.filename << std::endl;
                }
            }
        }

        auto bindAnim = [](Direction d, AnimFrame f, const std::string& tsvKey) {
            animLookupTable[(int)d][(int)f] = &textureCache[tsvKey];
            keyLookupTable[(int)d][(int)f] = tsvKey;
            };

        bindAnim(Direction::Down, AnimFrame::Walk1, "player_wdl");
        bindAnim(Direction::Down, AnimFrame::Stand, "player_sd");
        bindAnim(Direction::Down, AnimFrame::Walk2, "player_wdr");

        bindAnim(Direction::Up, AnimFrame::Walk1, "player_wul");
        bindAnim(Direction::Up, AnimFrame::Stand, "player_su");
        bindAnim(Direction::Up, AnimFrame::Walk2, "player_wur");

        bindAnim(Direction::Left, AnimFrame::Walk1, "player_wll");
        bindAnim(Direction::Left, AnimFrame::Stand, "player_sl");
        bindAnim(Direction::Left, AnimFrame::Walk2, "player_wlr");

        bindAnim(Direction::Right, AnimFrame::Walk1, "player_wrl");
        bindAnim(Direction::Right, AnimFrame::Stand, "player_sr");
        bindAnim(Direction::Right, AnimFrame::Walk2, "player_wrr");

        std::cout << "[Engine Core] Data-Driven Runtime Address Mapping Success.\n";
    }
}