#ifndef ALCHEMY_MODE_H
#define ALCHEMY_MODE_H

#include "GameMode.h"
#include "PotionFactory.h"
#include "ParticleTemplates.h"
#include "EffectDisplay.h"
#include <vector>
#include <memory>
#include <SFML/Audio.hpp> // Added for SoundBuffer/Sound

class AlchemyMode : public GameModeDecorator {
    std::vector<std::unique_ptr<FallingItemBase>> potions;
    
    // Particle Systems (Templates)
    TemplateParticleSystem<PotionSplashTrait> potionSplashSystem;
    TemplateParticleSystem<WitchMagicTrait> witchMagicSystem;
    
    float spawnTimer;
    float spawnInterval;
    
    sf::SoundBuffer breakBuffer;
    sf::Sound breakSound;
    
    sf::SoundBuffer spawnBuffer; // bow_shot when potion spawns
    sf::Sound spawnSound;
    
    std::vector<sf::SoundBuffer> glassDigBuffers; // glass_dig1/2/3 for break
    
    Grid* grid{nullptr}; 
    mutable sf::Vector2u windowSize{1920, 1080}; // Track window size (mutable to update in draw)
    mutable sf::Vector2f mousePos{0, 0}; // Track mouse position (updated in draw)
    
    float hitboxRadius{25.f}; // Mouse hitbox radius (smaller)
    sf::Vector2f hitboxOffset{16.f, 16.f}; // Offset to center on custom cursor texture (right + down)
    bool showHitbox{false}; // Debug: hide hitbox visually
    
    // Active Effects Management
    std::vector<ActiveEffect> activeEffects;
    mutable EffectDisplay effectDisplay; // Display active potion effects (mutable for draw in const method)
    
    // Mouse tracking for cursor effects
    mutable sf::Vector2i lastMousePos;
    mutable bool mouseInitialized{false};
    mutable sf::RenderWindow* cachedWindow{nullptr};
    
    // Effect tick timers
    mutable float effectTickTimer{0.0f};
    static constexpr float EFFECT_TICK_RATE = 2.0f; // Tick every 2 seconds
    
public:
    explicit AlchemyMode(std::unique_ptr<GameMode> mode);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    bool handleInput(const sf::Event& event, const sf::RenderWindow& window) override;
    
    // Pure Virtuals Implementations
    bool isLost() const override { return getMistakes() >= getMaxMistakes(); }
    
    int getMaxMistakes() const override { return 20; } // 10 hearts (20 half-hearts)
    std::string getName() const override { return "Alchemy Mode"; }
    std::unique_ptr<GameMode> clone() const override {
        return std::make_unique<AlchemyMode>(wrappedMode->clone()); 
    }

    void print(std::ostream &os) const override {
        os << "AlchemyMode + ";
        GameModeDecorator::print(os);
    }
    
    void setGrid(Grid* g) override { 
        this->grid = g;
        GameModeDecorator::setGrid(g);
    }
    
    void setSfxVolume(float volume) override {
        GameModeDecorator::setSfxVolume(volume);
        breakSound.setVolume(volume);
    }
    
    // Effect management
    const std::vector<ActiveEffect>& getActiveEffects() const { return activeEffects; }
    void addEffect(EffectType type, float duration);
    void updateEffects(float deltaTime);
    
    // Effect checkers
    bool hasEffect(EffectType type) const;
    float getCursorSpeedMultiplier() const;
    sf::Vector2f getLevitationOffset(float deltaTime) const;
    
private:
    static EffectType colorToEffect(const sf::Color& color);
};

#endif // ALCHEMY_MODE_H
