//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_GRIDRENDERER_H
#define OOP_GRIDRENDERER_H

#include <SFML/Graphics.hpp>
#include "Grid.h"

class GridRenderer {
    Grid& grid;
    float cellSize;
    sf::Vector2f offset;
    sf::Font font;
    bool fontLoaded;

public:
    explicit GridRenderer(Grid& g, float size = 40.f, sf::Vector2f offset = {50.f, 50.f});

    ~GridRenderer() = default;

    void draw(sf::RenderWindow& window) const;
    void drawGameInfo(sf::RenderWindow& window) const;
    void handleClick(const sf::Vector2i& mousePos) const;
};

#endif //OOP_GRIDRENDERER_H