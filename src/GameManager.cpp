#include "GameManager.h"
#include <iostream>
#include <optional>

GameManager::GameManager() : grid(), inMenu(true) {
    auto &menuRes = MenuResolution::getInstance();
    sf::VideoMode mode = menuRes.selectResolution();

    if (menuRes.wasFullscreenChosen())
        window.create(mode, "PictoCraft", sf::State::Fullscreen);
    else
        window.create(mode, "PictoCraft", sf::Style::Close, sf::State::Windowed);

    window.setFramerateLimit(60);

    menu = std::make_unique<GameMenu>();
}

void GameManager::startGame() {
    bool useScoreMode = (menu->getGameMode() == GameMode::Score);

    if (menu->getSourceMode() == SourceMode::File) {
        grid.load_from_file(menu->getSelectedFile(), useScoreMode);
    } else {
        grid.generate_random(menu->getGridSize(), useScoreMode, 0.6);
    }

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
    inMenu = false;
}

void GameManager::run() {
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (inMenu) {
                menu->handleEvent(*event, window);

                // Verifică dacă utilizatorul vrea să iasă
                if (menu->shouldQuit()) {
                    window.close();
                    return; // Ieșire din funcție
                }

                if (menu->isGameReady()) {
                    startGame();
                }
            } else {
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
        }

        if (inMenu) {
            menu->draw(window);
        } else {
            window.clear(sf::Color(240, 240, 240));
            renderer->draw(window);
            renderer->drawGameInfo(window);
        }

        window.display();
    }
}