#ifndef POTION_FACTORY_H
#define POTION_FACTORY_H

#include "Potion.h"
#include <memory>
#include <vector>
#include <cstdint>
#include <SFML/Graphics.hpp>

class PotionFactory {
    sf::Texture bottleTex;
    sf::Texture overlayTex;
    
    // Private Constructor for Singleton
    PotionFactory();

    // Delete copy/move
    PotionFactory(const PotionFactory&) = delete;
    PotionFactory& operator=(const PotionFactory&) = delete;

public:
    static PotionFactory& getInstance() {
        static PotionFactory instance;
        return instance;
    }

    static sf::Color hexToColor(std::uint32_t hex);

    const std::vector<std::uint32_t> goodColors = {
        0x33EBFF, // Speed
        0xD9C043, // Haste
        0xF82423, // Instant Health
        0xFF69B4, // Saturation
        0xCD5CAB, // Regeneration
        0xC2FF66  // Night Vision
    };

    const std::vector<std::uint32_t> badColors = {
        0x8BAFE0, // Slowness
        0x4A4217, // Mining Fatigue
        0xA9656A, // Instant Damage
        0x1F1F23, // Blindness
        0x587653, // Hunger
        0x484D48, // Weakness
        0x87A363, // Poison
        0xCEFFFF  // Levitation
    };

    std::unique_ptr<FallingItemBase> createRandomPotion(sf::Vector2u windowSize);
    
    const sf::Texture& getBottleTex() const;
};

#endif // POTION_FACTORY_H
