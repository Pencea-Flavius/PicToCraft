#ifndef EFFECT_DISPLAY_H
#define EFFECT_DISPLAY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "NinePatch.h"

enum class EffectType {
    // Positive effects
    Haste,
    Regeneration,
    InstantHealth,
    Saturation,
    
    // Negative effects
    MiningFatigue,
    Poison,
    InstantDamage,
    Levitation,
    Slowness,
    Speed,
    Weakness,
    Hunger,
    
    // Special effects
    Blindness,
    NightVision
};

struct ActiveEffect {
    EffectType type;
    float duration;
    int amplifier;
};

class EffectDisplay {
public:
    EffectDisplay();

    void loadAssets();
    void draw(sf::RenderWindow& window, float uiScale, const std::vector<ActiveEffect>& activeEffects, std::optional<sf::Vector2f> startPos = std::nullopt);

private:
    sf::Texture backgroundTexture;
    NinePatch backgroundPatch;
    
    std::map<EffectType, sf::Texture> iconTextures;
    
    sf::Font font;
};

#endif // EFFECT_DISPLAY_H
