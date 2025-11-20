#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include "GameMenu.h"
#include "GameOverScreen.h"
#include "Grid.h"
#include "GridRenderer.h"
#include "MenuResolution.h"
#include <SFML/Graphics.hpp>
#include <memory>

class GameManager {
  sf::RenderWindow window;
  Grid grid;
  std::unique_ptr<GridRenderer> renderer;
  std::unique_ptr<GameMenu> menu;
  std::unique_ptr<GameOverScreen> gameOverScreen;

  bool inMenu;
  bool inGameOver;

public:
  GameManager();
  void run();

  ~GameManager() = default;

private:
  void startGame();
  void resetGame();
};

#endif // OOP_GAMEMANAGER_H