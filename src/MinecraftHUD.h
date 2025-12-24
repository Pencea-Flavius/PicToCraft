#ifndef MINECRAFT_HUD_H
#define MINECRAFT_HUD_H

#include <algorithm>
#include "HeartDisplay.h"
#include "HungerDisplay.h"
#include "XPBar.h"
#include "EffectDisplay.h"
#include <SFML/Graphics.hpp>

class MinecraftHUD {
    sf::Texture hotbarTexture;
    sf::Sprite hotbarSprite;
    
    HeartDisplay heartDisplay;
    HungerDisplay hungerDisplay;
    XPBar xpBar;
    EffectDisplay effectDisplay;
    
    bool showHearts;
    bool showHunger;
    
    int currentHunger; // 0-20 (half drumsticks)
    int maxHunger;
    int maxHealth; // Max health (10 = 5 hearts, 20 = 10 hearts)
    
    mutable int lastMistakes;
    
    // Mechanics
    float hungerTimer;    // Accumulates time for hunger decay
    float regenTimer;     // Accumulates time for saturation regen
    float damageTimer;    // Accumulates time for starvation damage
    
    static constexpr float HUNGER_TICK_RATE = 4.0f; // Slower hunger decay normally (Example)
    static constexpr float REGEN_TICK_RATE = 1.0f; // Fast regen with saturation
    static constexpr float DAMAGE_TICK_RATE = 4.0f; // 4 seconds interval for starvation damage

public:
    MinecraftHUD();
    
    // Returns true if player should take damage (starvation)
    bool update(float deltaTime, bool hasHungerEffect = false, bool hasSaturationEffect = false);
    
    // Configure visibility based on game mode
    void setShowHearts(bool show) { showHearts = show; }
    void setShowHunger(bool show) { showHunger = show; }
    void setMaxHealth(int maxHP) { maxHealth = maxHP; }
    
    // Hunger management
    void setHunger(int hunger) { currentHunger = std::clamp(hunger, 0, maxHunger); }
    int getHunger() const { return currentHunger; }
    void modifyHunger(int delta);
    void triggerHungerFlash() { hungerDisplay.triggerFlash(); }
    void triggerHealFlash() { heartDisplay.triggerFlash(); } // For regeneration effect
    
    // Draw the full HUD at bottom center of screen
    void draw(sf::RenderWindow& window, int score, int currentMistakes, int maxMistakes,
              bool isPoisoned = false, bool hasHungerEffect = false, bool isWithered = false);
};

#endif // MINECRAFT_HUD_H
