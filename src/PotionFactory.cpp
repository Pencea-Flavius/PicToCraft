#include "PotionFactory.h"
#include <cmath>
#include <random>

PotionFactory::PotionFactory() {
    if (!bottleTex.loadFromFile("assets/potion/splash_potion.png")) {
            sf::Image img; 
            img.resize({32, 32}, sf::Color(255, 255, 255, 100));
            (void)bottleTex.loadFromImage(img);
    }
    if (!overlayTex.loadFromFile("assets/potion/potion_overlay.png")) {
            sf::Image img; 
            img.resize({24, 24}, sf::Color::White);
            (void)overlayTex.loadFromImage(img);
    }
}

sf::Color PotionFactory::hexToColor(std::uint32_t hex) {
    return {
        static_cast<std::uint8_t>((hex >> 16) & 0xFF),
        static_cast<std::uint8_t>((hex >> 8) & 0xFF),
        static_cast<std::uint8_t>((hex) & 0xFF)
    };
}

std::unique_ptr<FallingItemBase> PotionFactory::createRandomPotion(sf::Vector2u windowSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> typeDist(0, 10);
    
    auto width = static_cast<float>(windowSize.x);
    auto height = static_cast<float>(windowSize.y);
    
    if (width == 0) width = 1280;
    if (height == 0) height = 720;

    // Calculate dynamic scale: Base 5.0f at 1440p
    float normalizedHeight = height / 1440.f;
    float scale = std::pow(normalizedHeight, 1.2f) * 5.0f;
    
    if (scale < 1.5f) scale = 1.5f; // Minimum size increased

    if (typeDist(gen) > 2) { // 70% bad potions (homing), 30% good (falling)
        // Good Potion: Top spawning (Falling)
        float padding = 100.f * (scale / 5.0f);
        if (width < 2 * padding) padding = 0; 
        
        std::uniform_real_distribution<float> distX(padding, width - padding);
        sf::Vector2f startPos(distX(gen), -150.f * (scale/5.f)); // Adjust spawn height too
        
        // Pick Random Good Color
        std::uniform_int_distribution<size_t> colorDist(0, goodColors.size() - 1);
        sf::Color c = hexToColor(goodColors[colorDist(gen)]);
        
        float driftX = std::uniform_real_distribution<float>(-50.f, 50.f)(gen) * (scale / 5.0f);
        return std::make_unique<Potion<GravityPolicy>>(bottleTex, overlayTex, startPos, c, scale, sf::Vector2f(driftX, 100.f * (scale / 5.0f)));
    } else {
        // Bad Potion: Linear Fly-Through
        std::uniform_int_distribution<> edgeDist(0, 3);
        int edge = edgeDist(gen);
        float pad = 150.f * (scale/5.f);
        float x = 0, y = 0;
        
        switch(edge) {
            case 0: x = static_cast<float>(gen() % static_cast<int>(width)); y = -pad; break; // Top
            case 1: x = width + pad; y = static_cast<float>(gen() % static_cast<int>(height)); break; // Right
            case 2: x = static_cast<float>(gen() % static_cast<int>(width)); y = height + pad; break; // Bottom
            case 3: x = -pad; y = static_cast<float>(gen() % static_cast<int>(height)); break; // Left
            default: x = 0; y = -pad; break; 
        }
        
        sf::Vector2f startPos(x, y);
        
        // Initial velocity toward center for launch
        sf::Vector2f dir = sf::Vector2f(width/2.f, height/2.f) - startPos;
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if(len > 0) dir /= len;
        
        float baseSpeed = std::uniform_real_distribution<float>(500.f, 800.f)(gen); 
        float speed = baseSpeed * (scale / 5.0f); // Scale speed
        sf::Vector2f vel = dir * speed;
        
        // Pick Random Bad Color
        std::uniform_int_distribution<size_t> colorDist(0, badColors.size() - 1);
        sf::Color c = hexToColor(badColors[colorDist(gen)]);
        
        return std::make_unique<Potion<HomingPolicy>>(bottleTex, overlayTex, startPos, c, scale, vel);
    }
}


