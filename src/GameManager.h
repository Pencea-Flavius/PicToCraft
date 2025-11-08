#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Grid.h"
#include "GridRenderer.h"
#include "MenuResolution.h"

class GameManager {
    sf::RenderWindow window;
    Grid grid;
    std::unique_ptr<GridRenderer> renderer;  // pointer pentru reconstrucție

public:
    GameManager();
    void run();
};

#endif // OOP_GAMEMANAGER_H
