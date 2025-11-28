#ifndef OOP_SPIDERSMODE_H
#define OOP_SPIDERSMODE_H

#include "GameMode.h"
#include <SFML/Graphics.hpp>
#include <vector>

#include "Spider.h"

class SpidersMode : public GameModeDecorator {
public:
    explicit SpidersMode(std::unique_ptr<GameMode> mode);

    ~SpidersMode() override = default;

    void update(float deltaTime) override;

    void draw(sf::RenderWindow &window) const override;

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

    float spawnTimer;
    float damageTimer; // For hold-to-break
    sf::Vector2i lastMousePos; // Store last mouse position
    mutable sf::Vector2u windowSize; // Cache window size for updates

    Grid *grid = nullptr;
    const GridRenderer *renderer = nullptr;

    void spawnSpider();
};

#endif // OOP_SPIDERSMODE_H