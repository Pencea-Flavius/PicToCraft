//
// Created by zzfla on 11/8/2025.
//

#include "GridRenderer.h"


GridRenderer::GridRenderer(Grid& g, float size, sf::Vector2f off)
    : grid(g), cellSize(size), offset(off) {}

void GridRenderer::draw(sf::RenderWindow& window) const {
    int n = grid.get_size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sf::RectangleShape rect(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
            rect.setPosition({
                offset.x + static_cast<float>(j) * cellSize,
                offset.y + static_cast<float>(i) * cellSize
            });

            if (grid.get_block(i, j).is_completed())
                rect.setFillColor(sf::Color::Black);
            else
                rect.setFillColor(sf::Color::White);

            rect.setOutlineColor(sf::Color(180, 180, 180));
            rect.setOutlineThickness(1.f);
            window.draw(rect);
        }
    }
}

void GridRenderer::handleClick(const sf::Vector2i& mousePos) const {
    int n = grid.get_size();
    int x = static_cast<int>(
          (static_cast<float>(mousePos.y) - offset.y) / cellSize
      );
    int y = static_cast<int>(
        (static_cast<float>(mousePos.x) - offset.x) / cellSize);
    if (x >= 0 && x < n && y >= 0 && y < n)
        grid.toggle_block(x, y);
}

void GridRenderer::zoom(float delta) {
    cellSize = std::max(10.f, cellSize + delta);
}
