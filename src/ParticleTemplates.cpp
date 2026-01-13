#include "ParticleTemplates.h"
#include <random>
#include <cmath>


namespace ParticleUtils {
    float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }
}

void PotionSplashTrait::init(TemplateParticle& p, sf::Color baseColor, float scale) {
    float angle = ParticleUtils::randomFloat(0.f, 6.28318f);
    float speed = ParticleUtils::randomFloat(100.f, 300.f) * scale; // Scale speed
    p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
    
    p.maxLifetime = ParticleUtils::randomFloat(0.5f, 1.0f);
    p.lifetime = p.maxLifetime;
    
    p.color = baseColor;
    p.size = ParticleUtils::randomFloat(15.f, 30.f) * scale; // Scale size
    p.rotation = ParticleUtils::randomFloat(0.f, 360.f);
    p.angularVelocity = ParticleUtils::randomFloat(-180.f, 180.f);
}

void PotionSplashTrait::update(TemplateParticle& p, float dt) {
    p.velocity.y += 400.f * dt; 
    p.rotation += p.angularVelocity * dt;
}

sf::BlendMode PotionSplashTrait::getBlendMode() { 
    return sf::BlendAlpha; 
}

void WitchMagicTrait::init(TemplateParticle& p, sf::Color , float scale) {
    float angle = ParticleUtils::randomFloat(0.f, 6.28318f);
    float speed = ParticleUtils::randomFloat(10.f, 30.f) * scale;
    p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed - 20.f * scale};
    
    p.maxLifetime = ParticleUtils::randomFloat(1.5f, 3.0f);
    p.lifetime = p.maxLifetime;
    
    int r = static_cast<int>(ParticleUtils::randomFloat(100, 160));
    int b = static_cast<int>(ParticleUtils::randomFloat(180, 255));
    // Alpha 200
    p.color = sf::Color(static_cast<std::uint8_t>(r), 0, static_cast<std::uint8_t>(b), 200);
    
    p.size = ParticleUtils::randomFloat(5.f, 10.f) * scale;
    p.rotation = ParticleUtils::randomFloat(0.f, 360.f);
    p.angularVelocity = ParticleUtils::randomFloat(-90.f, 90.f);
}

void WitchMagicTrait::update(TemplateParticle& p, float dt) {
    p.position += p.velocity * dt;
    p.rotation += p.angularVelocity * dt;
    
    float ratio = p.lifetime / p.maxLifetime;
    // Use template lerp for alpha fade (Instantiation 2: float)
    float alpha = ParticleUtils::lerp(0.0f, 255.0f, ratio);
    p.color.a = static_cast<std::uint8_t>(alpha);
}

sf::BlendMode WitchMagicTrait::getBlendMode() { 
    return sf::BlendAdd; 
}

void DeathPoofTrait::init(TemplateParticle& p, sf::Color, float scale) {
    // Gaussian distribution for velocity
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, 1.0f);

    float xa = dist(gen) * 30.0f; 
    float ya = dist(gen) * 40.0f - 60.0f; 
    
    p.velocity = {xa * scale, ya * scale};

    float offsetX = dist(gen) * 40.0f * scale;
    float offsetY = dist(gen) * 40.0f * scale;
    
    p.position += sf::Vector2f(offsetX, offsetY);

    p.maxLifetime = ParticleUtils::randomFloat(0.5f, 1.5f); 
    p.lifetime = p.maxLifetime;
    
    p.color = sf::Color::White;
    p.size = ParticleUtils::randomFloat(20.f, 40.f) * scale;
    p.rotation = ParticleUtils::randomFloat(0.f, 360.f);
    p.angularVelocity = 0.f; 
}

void DeathPoofTrait::update(TemplateParticle& p, float dt) {
    p.position += p.velocity * dt;
}

sf::BlendMode DeathPoofTrait::getBlendMode() {
    return sf::BlendAlpha;
}
