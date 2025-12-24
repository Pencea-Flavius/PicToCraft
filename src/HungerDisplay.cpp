#include "HungerDisplay.h"
#include "Exceptions.h"
#include <random>
#include <algorithm>

HungerDisplay::HungerDisplay() 
    : isFlashing(false), flashTimer(0.0f),
      shakeTimer(0.0f), shakeIntervalTimer(0.0f),
      isShaking(false), hasHungerEffectState(false) {
    if (!emptyTexture.loadFromFile("assets/hunger/food_empty.png")) {
        throw AssetLoadException("assets/hunger/food_empty.png", "Texture");
    }
    if (!emptyHungerTexture.loadFromFile("assets/hunger/food_empty_hunger.png")) {
        throw AssetLoadException("assets/hunger/food_empty_hunger.png", "Texture");
    }
    if (!fullTexture.loadFromFile("assets/hunger/food_full.png")) {
        throw AssetLoadException("assets/hunger/food_full.png", "Texture");
    }
    if (!fullHungerTexture.loadFromFile("assets/hunger/food_full_hunger.png")) {
        throw AssetLoadException("assets/hunger/food_full_hunger.png", "Texture");
    }
    if (!halfTexture.loadFromFile("assets/hunger/food_half.png")) {
        throw AssetLoadException("assets/hunger/food_half.png", "Texture");
    }
    if (!halfHungerTexture.loadFromFile("assets/hunger/food_half_hunger.png")) {
        throw AssetLoadException("assets/hunger/food_half_hunger.png", "Texture");
    }

    emptySprite.emplace(emptyTexture);
    fullSprite.emplace(fullTexture);
    halfSprite.emplace(halfTexture);
}

void HungerDisplay::update(float deltaTime, bool hasHungerEffect) {
    hasHungerEffectState = hasHungerEffect;

    if (isFlashing) {
        flashTimer -= deltaTime;
        if (flashTimer <= 0.0f) {
            isFlashing = false;
            flashTimer = 0.0f;
        }
    }
    
    // Shake logic
    if (hasHungerEffect) {
        if (!isShaking) {
            shakeIntervalTimer += deltaTime;
            if (shakeIntervalTimer >= 3.0f) { // Shake every ~3 seconds
                isShaking = true;
                shakeTimer = 0.25f; // Shake for 0.25s
                shakeIntervalTimer = 0.0f;
            }
        } else {
            shakeTimer -= deltaTime;
            if (shakeTimer <= 0.0f) {
                isShaking = false;
                shakeTimer = 0.0f;
            }
        }
    } else {
        isShaking = false;
        shakeTimer = 0.0f;
        shakeIntervalTimer = 0.0f;
    }
}

void HungerDisplay::triggerFlash() {
    isFlashing = true;
    flashTimer = FLASH_DURATION;
}

void HungerDisplay::draw(sf::RenderWindow& window, int currentHunger, int maxHunger,
                         const sf::Vector2f& position, float scale, bool hasHungerEffect) {
    
    bool useHungerTexture = hasHungerEffect && (static_cast<int>(flashTimer * 5.0f) % 2 == 0);
    
    if (useHungerTexture || hasHungerEffect) {
        emptySprite->setTexture(emptyHungerTexture);
        fullSprite->setTexture(fullHungerTexture);
        halfSprite->setTexture(halfHungerTexture);
    } else {
        emptySprite->setTexture(emptyTexture);
        fullSprite->setTexture(fullTexture);
        halfSprite->setTexture(halfTexture);
    }

    emptySprite->setScale({scale, scale});
    fullSprite->setScale({scale, scale});
    halfSprite->setScale({scale, scale});

    int totalHalfDrumsticks = maxHunger;
    int currentHalfDrumsticks = std::clamp(currentHunger, 0, totalHalfDrumsticks);
    int totalDrumsticks = (totalHalfDrumsticks + 1) / 2;
    float drumstickSpacing = 8.0f * scale;

    bool isLowHunger = (static_cast<float>(currentHalfDrumsticks) / static_cast<float>(totalHalfDrumsticks)) <= 0.2f ||
                       currentHalfDrumsticks <= 1;

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // Draw from RIGHT to LEFT
    for (int i = 0; i < totalDrumsticks; ++i) {
        // Position from right: (totalDrumsticks - 1 - i) places it at right side
        float x = position.x - static_cast<float>(i + 1) * drumstickSpacing;
        float y = position.y;

        bool isZeroHungerShake = (currentHunger == 0);
        
        if ((isLowHunger || isShaking || isZeroHungerShake) && (hasHungerEffect || isZeroHungerShake)) {
            float shakeOffset = dist(rng) * scale;
            y += shakeOffset;
        }

        // Draw empty container first
        emptySprite->setPosition({x, y});
        window.draw(*emptySprite);

        
        int drumstickIndex = i;
        int firstHalfIndex = drumstickIndex * 2;
        int secondHalfIndex = drumstickIndex * 2 + 1;

        int drumstickValue = 0;
        if (currentHalfDrumsticks > secondHalfIndex) {
            drumstickValue = 2; // Full
        } else if (currentHalfDrumsticks > firstHalfIndex) {
            drumstickValue = 1; // Half
        }

        if (drumstickValue == 2) {
            fullSprite->setPosition({x, y});
            window.draw(*fullSprite);
        } else if (drumstickValue == 1) {
            halfSprite->setPosition({x, y});
            window.draw(*halfSprite);
        }
    }
}
