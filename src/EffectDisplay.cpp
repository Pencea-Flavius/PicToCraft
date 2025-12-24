#include "EffectDisplay.h"
#include "Exceptions.h"
#include <iostream>
#include <cstdio>
#include <cmath>

EffectDisplay::EffectDisplay() = default;

void EffectDisplay::loadAssets() {
    if (!backgroundTexture.loadFromFile("assets/effect_background.png")) {
        throw AssetLoadException("assets/effect_background.png", "Texture");
    }
    
    backgroundPatch = NinePatch(backgroundTexture, 6); 
    
    struct IconInfo { EffectType type; std::string filename; };
    std::vector<IconInfo> icons = {
        {EffectType::Haste, "haste.png"},
        {EffectType::Regeneration, "regeneration.png"},
        {EffectType::MiningFatigue, "mining_fatigue.png"},
        {EffectType::Poison, "poison.png"},
        {EffectType::Levitation, "levitation.png"},
        {EffectType::Slowness, "slowness.png"},
        {EffectType::Speed, "speed.png"},
        {EffectType::Weakness, "weakness.png"},
        {EffectType::Hunger, "hunger.png"},
        {EffectType::Saturation, "saturation.png"},
        {EffectType::Blindness, "blindness.png"},
        {EffectType::NightVision, "night_vision.png"}
    };
    
    for (const auto& iconInfo : icons) {
        sf::Texture tex;
        std::string path = "assets/mob_effect/" + iconInfo.filename;
        if (!tex.loadFromFile(path)) {
            std::cerr << "Warning: Failed to load " << path << std::endl;
        } else {
            iconTextures[iconInfo.type] = tex;
        }
    }
    
    if (!font.openFromFile("assets/Monocraft.ttf")) {
        std::cerr << "Warning: Failed to load assets/Monocraft.ttf" << std::endl;
    }
}

void EffectDisplay::draw(sf::RenderWindow& window, float uiScale, const std::vector<ActiveEffect>& activeEffects, std::optional<sf::Vector2f> startPos) {
    if (activeEffects.empty()) return;

    auto winSize = window.getSize();
    // float padding = 4.0f * uiScale;
    
    float startX, startY;
    if (startPos.has_value()) {
        startX = startPos->x;
        startY = startPos->y;
    } else {
        float padding = 4.0f * uiScale;
        startX = static_cast<float>(winSize.x) - padding; 
        startY = padding;
    }
    
    backgroundPatch.setPatchScale(uiScale);

    for (size_t i = 0; i < activeEffects.size(); ++i) {
        const auto& effect = activeEffects[i];
        
        std::string nameStr;
        std::string durationStr;
        
        switch (effect.type) {
            case EffectType::Haste: nameStr = "Haste"; break;
            case EffectType::Regeneration: nameStr = "Regeneration"; break;
            case EffectType::InstantHealth: nameStr = "Instant Health"; break;
            case EffectType::Saturation: nameStr = "Saturation"; break;
            case EffectType::MiningFatigue: nameStr = "Mining Fatigue"; break;
            case EffectType::Poison: nameStr = "Poison"; break;
            case EffectType::InstantDamage: nameStr = "Instant Damage"; break;
            case EffectType::Levitation: nameStr = "Levitation"; break;
            case EffectType::Slowness: nameStr = "Slowness"; break;
            case EffectType::Speed: nameStr = "Speed"; break;
            case EffectType::Weakness: nameStr = "Weakness"; break;
            case EffectType::Hunger: nameStr = "Hunger"; break;
            case EffectType::Blindness: nameStr = "Blindness"; break;
            case EffectType::NightVision: nameStr = "Night Vision"; break;
            default: nameStr = "Unknown"; break;
        }
        
        if (std::isinf(effect.duration) || effect.duration > 9999.0f) {
            durationStr = "Infinite";
        } else {
            int totalSeconds = static_cast<int>(effect.duration);
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            char buffer[16];
            std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
            durationStr = buffer;
        }
        
        auto fontSize = static_cast<unsigned int>(15.0f * uiScale); 
        const_cast<sf::Texture&>(font.getTexture(fontSize)).setSmooth(false);
        
        sf::Text nameText(font, nameStr, fontSize);
        sf::Text durText(font, durationStr, fontSize);
        
        float nameW = nameText.getLocalBounds().size.x;
        float durW = durText.getLocalBounds().size.x;
        float maxTextW = std::max(nameW, durW);
        
        float baseHeight = 50.0f;
        float boxWidthVal = 50.0f + (maxTextW / uiScale) + 15.0f; 
        
        if (boxWidthVal < 120.0f) boxWidthVal = 120.0f;
        
        float boxW = boxWidthVal * uiScale;
        float boxH = baseHeight * uiScale;
        
        float x;
        if (startPos.has_value()) {
            x = startX; 
        } else {
            x = startX - boxW;
        }
        
        float verticalPadding = 4.0f * uiScale;
        float y = startY + (static_cast<float>(i) * (boxH + verticalPadding));      

        backgroundPatch.setSize(boxW, boxH);
        backgroundPatch.setPosition({x, y});
        window.draw(backgroundPatch);
        
        if (iconTextures.contains(effect.type)) {
            const sf::Texture& texture = iconTextures.at(effect.type);
            sf::Sprite icon(texture);
            
            float iconSize = 30.0f;

            auto texSize = texture.getSize();
            float scaleX = (iconSize * uiScale) / static_cast<float>(texSize.x);
            float scaleY = (iconSize * uiScale) / static_cast<float>(texSize.y);
            
            icon.setScale({scaleX, scaleY});
            
            float iconSquareSize = 50.0f * uiScale;
            float scaledIconSize = iconSize * uiScale;
            float centerOffset = (iconSquareSize - scaledIconSize) / 2.0f;
            
            icon.setPosition({x + centerOffset, y + centerOffset});
            window.draw(icon);
        }
        
        float textLeftPad = 4.0f;
        float textX = x + 50.0f * uiScale + textLeftPad * uiScale; 
        float textY1 = y + 7.0f * uiScale;  
        float textY2 = y + 25.0f * uiScale; 
        
        nameText.setPosition({textX, textY1});
        durText.setPosition({textX, textY2});
        
        durText.setFillColor(sf::Color(180, 180, 180)); 
        
        window.draw(nameText);
        window.draw(durText);
    }
}
