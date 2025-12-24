#ifndef OOP_SPIDERSMODE_H
#define OOP_SPIDERSMODE_H

#include "GameMode.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "Spider.h"

class SpidersMode : public GameModeDecorator {
public:
    explicit SpidersMode(std::unique_ptr<GameMode> mode);

    ~SpidersMode() override = default;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow &window) const override;
  
    void setSfxVolume(float volume) override;

    bool handleInput(const sf::Event &event,
                     const sf::RenderWindow &window) override;

    [[nodiscard]] std::unique_ptr<GameMode> clone() const override;

    [[nodiscard]] std::string getName() const override { return "Spiders Mode"; }

    void print(std::ostream &os) const override {
        os << "SpidersMode + ";
        GameModeDecorator::print(os);
    }

    void setGrid(Grid *g) override;

    void setRenderer(const GridRenderer *r) override;

private:
    std::vector<Spider> spiders;
    sf::Texture walkTexture;
    sf::Texture idleTexture;
    sf::Texture deathTexture;

    sf::SoundBuffer deathBuffer;
    std::vector<sf::SoundBuffer> idleBuffers;
    std::vector<sf::SoundBuffer> stepBuffers;
    
    sf::SoundBuffer brokenWebBuffer;
    std::vector<sf::SoundBuffer> hitWebBuffers;
    
    // Use a separate audio source for web interaction to avoid cutting off spiders maybe? 
    // Or just one for logic. Let's start with one on the mode or use fire-and-forget logic if possible?
    // Actually SpidersMode shouldn't probably own the audioSource for *spiders*, but it needs one for *webs*.
    std::optional<sf::Sound> webAudioSource;

    float spawnTimer;
    float damageTimer; // For hold-to-break
    float currentVolume = 100.0f;
    sf::Vector2i lastMousePos; // Store last mouse position
    mutable sf::Vector2u windowSize; // Cache window size for updates

    Grid *grid = nullptr;
    const GridRenderer *renderer = nullptr;
    
    bool wasWeaknessActive = false; // Track Weakness effect state

    void spawnSpider();
};

#endif // OOP_SPIDERSMODE_H