#include "MinecraftHUD.h"
#include "Exceptions.h"
#include <algorithm>
#include <iostream>

MinecraftHUD::MinecraftHUD() 
    : hotbarSprite(hotbarTexture), showHearts(true), showHunger(true),
      currentHunger(20), maxHunger(20), maxHealth(10), // Default 5 hearts
      lastMistakes(0),
      hungerTimer(0.f), regenTimer(0.f), damageTimer(0.f) {
    
    if (!hotbarTexture.loadFromFile("assets/hotbar.png")) {
        throw AssetLoadException("assets/hotbar.png", "Texture");
    } else {
        hotbarSprite.setTexture(hotbarTexture);
    }
    
    effectDisplay.loadAssets();
}

bool MinecraftHUD::update(float deltaTime, bool hasHungerEffect, bool hasSaturationEffect) {
    heartDisplay.update(deltaTime);
    hungerDisplay.update(deltaTime, hasHungerEffect);
    
    bool takeDamage = false;
    
    // Hunger only decays if Hunger effect is active
    if (hasHungerEffect) {
        hungerTimer += deltaTime;
        if (hungerTimer >= HUNGER_TICK_RATE) {
            hungerTimer = 0.0f;
            modifyHunger(-1); // Decay hunger
        }
    } else {
        hungerTimer = 0.f; // Reset timer if hunger effect is not active
    }

    if (hasSaturationEffect) {
        regenTimer += deltaTime;
        if (regenTimer >= REGEN_TICK_RATE) {
            modifyHunger(1); // Saturation restores hunger
            regenTimer = 0.f;
        }
    } else {
        regenTimer = 0.f; // Reset timer if saturation effect is not active
    }
    
    // Starvation damage
    if (currentHunger == 0) {
        damageTimer += deltaTime;
        if (damageTimer >= DAMAGE_TICK_RATE) {
            takeDamage = true;
            damageTimer = 0.f;
        }
    } else {
        damageTimer = 0.f;
    }
    
    return takeDamage;
}

void MinecraftHUD::modifyHunger(int delta) {
    int oldHunger = currentHunger;
    currentHunger = std::clamp(currentHunger + delta, 0, maxHunger);
    if (currentHunger < oldHunger) {
        hungerDisplay.triggerFlash();
    }
}

void MinecraftHUD::draw(sf::RenderWindow& window, int score, int currentMistakes, int maxMistakes,
                        bool isPoisoned, bool hasHungerEffect, bool isWithered) {
    auto winSize = window.getSize();
    
    float scaleX = static_cast<float>(winSize.x) / 1280.0f;
    float scaleY = static_cast<float>(winSize.y) / 720.0f;
    float uiScale = std::min(scaleX, scaleY);
    
    float bottomPadding = 0.f; 
    float centerX = static_cast<float>(winSize.x) / 2.f;
    float hotbarY = static_cast<float>(winSize.y) - bottomPadding;
    

    auto hotbarSize = hotbarTexture.getSize();
    float hotbarScale = uiScale * 2.5f;
    float hotbarHeight = 22.f * hotbarScale;
    float hotbarWidth = 182.f * hotbarScale;
    
    if (hotbarSize.x > 0 && hotbarSize.y > 0) {
        hotbarHeight = static_cast<float>(hotbarSize.y) * hotbarScale;
        hotbarWidth = static_cast<float>(hotbarSize.x) * hotbarScale;
        
        hotbarSprite.setColor(sf::Color::White); 
        hotbarSprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(static_cast<int>(hotbarSize.x), static_cast<int>(hotbarSize.y))));
        
        hotbarSprite.setScale({hotbarScale, hotbarScale});
        hotbarSprite.setOrigin({static_cast<float>(hotbarSize.x) / 2.f, static_cast<float>(hotbarSize.y)});
        hotbarSprite.setPosition({centerX, hotbarY});
        window.draw(hotbarSprite);
    }
    
    // XP BAR
    float xpBarY = hotbarY - hotbarHeight - 10.f * uiScale;
    xpBar.draw(window, score, {centerX, xpBarY}, hotbarScale);
    
    // HEARTS AND HUNGER
    float statsY = xpBarY - 32.f * uiScale;
    
    if (currentMistakes > lastMistakes) {
        heartDisplay.triggerFlash();
    } else if (currentMistakes < lastMistakes) {
        heartDisplay.triggerShake();
    }
    lastMistakes = currentMistakes;
    
    if (showHearts) {
        float heartsX = centerX - hotbarWidth / 2.f;
        heartDisplay.draw(window, currentMistakes, maxHealth, {heartsX, statsY}, hotbarScale, isPoisoned, isWithered);
    }
    
    if (showHunger) {
        float hungerX = centerX + hotbarWidth / 2.f;
        hungerDisplay.draw(window, currentHunger, maxHunger, {hungerX, statsY}, hotbarScale, hasHungerEffect);
    }
    
}


