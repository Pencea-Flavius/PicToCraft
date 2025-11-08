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
        window.create(mode, "Pictocross",  sf::Style::Close , sf::State::Windowed);

    window.setFramerateLimit(60);

    grid.load_from_file("item.txt");

    auto winSize = window.getSize();
    int n = grid.get_size();
    float gridDisplaySize = std::min(winSize.x, winSize.y) * 0.7f;
    float cellSize = gridDisplaySize / static_cast<float>(n);

    float offsetX = (winSize.x - gridDisplaySize) / 2.f;
    float offsetY = (winSize.y - gridDisplaySize) / 2.f;

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

        window.clear(sf::Color::White);
        renderer->draw(window);
        window.display();
    }
}
