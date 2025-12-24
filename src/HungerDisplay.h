#ifndef HUNGER_DISPLAY_H
#define HUNGER_DISPLAY_H

#include <SFML/Graphics.hpp>
#include <optional>

class HungerDisplay {
    sf::Texture emptyTexture;
    sf::Texture emptyHungerTexture;
    sf::Texture fullTexture;
    sf::Texture fullHungerTexture;
    sf::Texture halfTexture;
    sf::Texture halfHungerTexture;

    std::optional<sf::Sprite> emptySprite;
    std::optional<sf::Sprite> fullSprite;
    std::optional<sf::Sprite> halfSprite;

    bool isFlashing;
    float flashTimer;
    const float FLASH_DURATION = 1.0f;
    
    // Shake effect for hunger
    float shakeTimer;
    float shakeIntervalTimer;
    bool isShaking;
    bool hasHungerEffectState; // To track if we should be shaking occasionally

public:
    HungerDisplay();

    void update(float deltaTime, bool hasHungerEffect); // Updated signature
    void triggerFlash();
    
    // Draw hunger from RIGHT to LEFT (opposite of hearts)
    // currentHunger: 0-20 (half drumsticks), maxHunger: typically 20
    void draw(sf::RenderWindow& window, int currentHunger, int maxHunger,
              const sf::Vector2f& position, float scale, bool hasHungerEffect = false);
};

#endif // HUNGER_DISPLAY_H
