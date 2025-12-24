#include "Potion.h"
#include "ParticleTemplates.h" // For ParticleUtils::lerp
#include <cmath>

void GravityPolicy::update(sf::Vector2f& pos, sf::Vector2f& vel, float dt, const sf::Vector2f& /*target*/, float scale, float& /*timer*/) {
    // Scale gravity relative to base scale 5.0
    float gravity = 500.f * (scale / 5.0f); 
    vel.y += gravity * dt; 
    pos += vel * dt;
}

bool GravityPolicy::isBad() { 
    return false; 
}

void HomingPolicy::update(sf::Vector2f& pos, sf::Vector2f& vel, float dt, const sf::Vector2f& target, float scale, float& homingTime) {
    float speed = 800.f * (scale / 5.0f);
    float homingStrength = 3.f; 
    
    homingTime -= dt;
    if (homingTime <= 0.f) {
        homingStrength = 0.f; 
    }

    sf::Vector2f desiredDir = target - pos;
    float len = std::sqrt(desiredDir.x * desiredDir.x + desiredDir.y * desiredDir.y);
    if (len != 0) desiredDir /= len;

    sf::Vector2f currentDir = vel;
    float velocityLength = std::sqrt(currentDir.x * currentDir.x + currentDir.y * currentDir.y);
    if (velocityLength != 0) currentDir /= velocityLength;

    sf::Vector2f newDir = ParticleUtils::lerp(currentDir, desiredDir, homingStrength * dt);

    float newLength = std::sqrt(newDir.x * newDir.x + newDir.y * newDir.y);
    if (newLength != 0) newDir /= newLength;

    // 4. Apply
    vel = newDir * speed;
    pos += vel * dt;
}

bool HomingPolicy::isBad() { 
    return true; 
}
