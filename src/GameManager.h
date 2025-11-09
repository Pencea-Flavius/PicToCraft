#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Grid.h"
#include "GridRenderer.h"
#include "MenuResolution.h"
#include "GameMenu.h"

class GameManager {
    sf::RenderWindow window;
    Grid grid;
    std::unique_ptr<GridRenderer> renderer;
    std::unique_ptr<GameMenu> menu;
    bool inMenu;

public:
    GameManager();
    void run();

    ~GameManager() = default;

private:
    void startGame();
};

#endif // OOP_GAMEMANAGER_H