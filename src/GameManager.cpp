#include "GameManager.h"
#include <iostream>
#include <optional>

GameManager::GameManager()
    : grid()
{
    auto &menu = MenuResolution::getInstance();
    sf::VideoMode mode = menu.selectResolution();

    if (menu.wasFullscreenChosen())
        window.create(mode, "Pictocross", sf::State::Fullscreen);
    else
        window.create(mode, "Pictocross", sf::Style::Close, sf::State::Windowed);

    window.setFramerateLimit(60);

    grid.load_from_file("mob.txt");

    auto winSize = window.getSize();
    int n = grid.get_size();

    const auto& hints = grid.get_hints();
    size_t maxRowWidth = hints.get_max_row_width();
    size_t maxColHeight = hints.get_max_col_height();

    float availableWidth = winSize.x * 0.85f;
    float availableHeight = winSize.y * 0.85f;

    float cellSizeByWidth = availableWidth / (n + maxRowWidth * 0.8f);
    float cellSizeByHeight = availableHeight / (n + maxColHeight * 0.8f);
    float cellSize = std::min(cellSizeByWidth, cellSizeByHeight);

    float totalWidth = (n + maxRowWidth * 0.8f) * cellSize;
    float totalHeight = (n + maxColHeight * 0.8f) * cellSize;

    float offsetX = (winSize.x - totalWidth) / 2.f;
    float offsetY = (winSize.y - totalHeight) / 2.f;

    renderer = std::make_unique<GridRenderer>(grid, cellSize, sf::Vector2f(offsetX, offsetY));
}

void GameManager::run() {
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto m = event->getIf<sf::Event::MouseButtonPressed>();
                if (m && m->button == sf::Mouse::Button::Left) {
                    renderer->handleClick(sf::Mouse::getPosition(window));

                    if (grid.is_solved()) {
                        std::cout << "Puzzle rezolvat! Felicitari!\n";
                        window.close();
                    } else if (grid.is_lost()) {
                        std::cout << "Ai pierdut jocul!\n";
                        window.close();
                    }
                }
            }
        }

        window.clear(sf::Color(240, 240, 240));
        renderer->draw(window);
        window.display();
    }
}