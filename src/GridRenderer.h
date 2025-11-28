//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_GRIDRENDERER_H
#define OOP_GRIDRENDERER_H

#include "Grid.h"
#include "HeartDisplay.h"
#include <SFML/Graphics.hpp>
#include <optional>

class GridRenderer {
    Grid &grid;
    float cellSize;
    sf::Vector2f offset;
    sf::Font font;
    bool fontLoaded;
    mutable HeartDisplay heartDisplay;
    mutable int lastMistakes;
    mutable sf::Clock animationClock;

    sf::Texture webTexture;
    std::vector<sf::Texture> breakTextures;

public:
    explicit GridRenderer(Grid &g, float size = 40.f,
                          sf::Vector2f offset = {50.f, 50.f});

    ~GridRenderer() = default;

    void draw(sf::RenderWindow &window) const;

    void drawGameInfo(sf::RenderWindow &window) const;

    void handleClick(const sf::Vector2i &mousePos) const;

    void handleHintClick(const sf::Vector2i &mousePos) const;

    sf::Vector2f getHintCenter(bool isRow, int line, int index) const;
};

#endif // OOP_GRIDRENDERER_H