#ifndef OOP_ENDERMANMODE_H
#define OOP_ENDERMANMODE_H

#include "GameMode.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <vector>

class EndermanMode : public GameModeDecorator {
public:
    struct Hitbox {
        float x, y, w, h, r;
    };

    struct BoundingBox {
        sf::FloatRect rect;
        float rotation;
        bool isDragging = false;
    };

    explicit EndermanMode(std::unique_ptr<GameMode> mode);

    ~EndermanMode() override = default;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow &window) const override;

    bool handleInput(const sf::Event &event,
                     const sf::RenderWindow &window) override;

    [[nodiscard]] std::unique_ptr<GameMode> clone() const override;

    [[nodiscard]] std::string getName() const override { return "Enderman Mode"; }

    [[nodiscard]] bool isLost() const override;

    void print(std::ostream &os) const override {
        os << "EndermanMode + ";
        GameModeDecorator::print(os);
    }

private:
    sf::Texture endermanTexture;
    sf::Texture jumpscareTexture;
    std::optional<sf::Sprite> endermanSprite;
    std::optional<sf::Sprite> jumpscareSprite;

    sf::SoundBuffer jumpscareSoundBuffer; // Will load Enderman_death.ogg
    std::optional<sf::Sound> jumpscareSound;

    sf::SoundBuffer stareSoundBuffer;
    std::optional<sf::Sound> stareSound;

    std::vector<sf::SoundBuffer> idleBuffers;
    std::optional<sf::Sound> idleSound;

    std::vector<sf::SoundBuffer> hurtBuffers;
    std::optional<sf::Sound> hurtSound;

    float spawnTimer;
    float spawnInterval;
    float endermanLifetime;
    float maxLifetime;

    int currentFrame;
    float animationTimer;
    float frameTime;

    bool endermanVisible;
    bool jumpscareActive;
    bool playerLost;
    bool needsRescaling; // Flag to ensure we scale to window size on first draw

    float hoverTimer;
    float endermanOpacity;

    float jumpscareTimer;
    int jumpscareFrame;
    float jumpscareFrameTime;

    static constexpr int ENDERMAN_COLS = 5;
    static constexpr int ENDERMAN_ROWS = 2;
    static constexpr int ENDERMAN_FRAMES = 10;
    static constexpr int ENDERMAN_FRAME_WIDTH = 635;
    static constexpr int ENDERMAN_FRAME_HEIGHT = 621;

    static const std::array<Hitbox, 9> ENDERMAN_HITBOXES;

    static constexpr int JUMPSCARE_COLS = 5;
    static constexpr int JUMPSCARE_ROWS = 9;
    static constexpr int JUMPSCARE_TOTAL_FRAMES = 44;
    static constexpr int JUMPSCARE_FRAME_WIDTH = 800;
    static constexpr int JUMPSCARE_FRAME_HEIGHT = 650;

    void updateEndermanAnimation(float deltaTime);

    void updateJumpscareAnimation(float deltaTime);

    void spawnEnderman();

    void triggerJumpscare();

    bool isMouseOverEnderman(const sf::RenderWindow &window) const;

    void playRandomIdleSound();

    void playRandomHurtSound();

    void updateStareSound(bool isHovering, float deltaTime);
};

#endif // OOP_ENDERMANMODE_H