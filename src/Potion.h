#ifndef POTION_H
#define POTION_H

#include <SFML/Graphics.hpp>
#include "Grid.h"

// Abstract Base Class
class FallingItemBase {
public:
    virtual ~FallingItemBase() = default;
    virtual void update(float dt, const sf::Vector2f& playerCursor, sf::Vector2u windowSize) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    [[nodiscard]] virtual bool checkCollision(const sf::Vector2f& point) const = 0;
    [[nodiscard]] virtual bool isDead() const = 0;
    virtual void onCollect(Grid& grid) = 0; 
    virtual void onHit(Grid& grid) = 0;    
    [[nodiscard]] virtual sf::Color getColor() const = 0;
    [[nodiscard]] virtual bool isBad() const = 0;
};

struct GravityPolicy {
    static void update(sf::Vector2f& pos, sf::Vector2f& vel, float dt, const sf::Vector2f& /*target*/, float scale, float& /*timer*/);
    [[nodiscard]] static bool isBad();
};

struct HomingPolicy {
    static void update(sf::Vector2f& pos, sf::Vector2f& vel, float dt, const sf::Vector2f& target, float scale, float& homingTime);
    [[nodiscard]] static bool isBad();
};

template <typename Policy>
class Potion : public FallingItemBase {
    sf::Sprite bottleSprite;
    sf::Sprite overlaySprite;
    sf::Vector2f velocity;
    sf::Color color;
    float scaleVal;
    float homingTimer = 0.6f; // Homing active for 0.6s
    bool dead = false;
    
public:
    Potion(const sf::Texture& bottleTex, const sf::Texture& overlayTex, sf::Vector2f startPos, sf::Color c, float scale, sf::Vector2f initVel = {0.f, 0.f}) 
        : bottleSprite(bottleTex), overlaySprite(overlayTex), velocity(initVel), color(c), scaleVal(scale) { 
        
        sf::FloatRect bounds = bottleSprite.getLocalBounds();
        bottleSprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
        overlaySprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});

        bottleSprite.setScale({scale, scale});
        overlaySprite.setScale({scale, scale});
        
        bottleSprite.setPosition(startPos);
        overlaySprite.setPosition(startPos);
        
        overlaySprite.setColor(color);
    }

    void update(float dt, const sf::Vector2f& playerCursor, sf::Vector2u windowSize) override {
        sf::Vector2f pos = bottleSprite.getPosition();
        Policy::update(pos, velocity, dt, playerCursor, scaleVal, homingTimer);
        bottleSprite.setPosition(pos);
        overlaySprite.setPosition(pos);
        
        // Dynamic bounds check based on window size + large margin
        float margin = 500.f * (scaleVal / 5.f);
        
        // Remove if far outside screen (left, right, bottom)
        // Check Top only if it's way above (allow spawning from top)
        if (pos.y > static_cast<float>(windowSize.y) + margin || 
            pos.x < -margin || 
            pos.x > static_cast<float>(windowSize.x) + margin ||
            pos.y < -margin * 2.0f) { // Allow spawn from top (-150), but kill if way too high up
             dead = true;
        }
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(bottleSprite);
        window.draw(overlaySprite);
    }

    [[nodiscard]] bool checkCollision(const sf::Vector2f& point) const override {
        sf::Transform inverse = bottleSprite.getInverseTransform();
        sf::Vector2f localPoint = inverse.transformPoint(point);
        return bottleSprite.getLocalBounds().contains(localPoint);
    }

    [[nodiscard]] bool isDead() const override { return dead; }
    
    void onCollect(Grid& grid) override {
        dead = true;
        if (color == sf::Color::Red) {
            grid.healWebs();
        }
    }

    void onHit(Grid& grid) override {
        dead = true;
        if (Policy::isBad()) {
           grid.damageWeb(false, 0, 0); 
        }
    }
    
    [[nodiscard]] sf::Color getColor() const override { return color; }
    [[nodiscard]] bool isBad() const override { return Policy::isBad(); }
};

#endif // POTION_H
