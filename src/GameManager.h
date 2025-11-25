#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include "CustomCursor.h"
#include "GameMenu.h"
#include "GameOverScreen.h"
#include "Grid.h"
#include "GridRenderer.h"
#include "MenuResolution.h"
#include "WinScreen.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

class GameManager {
  sf::RenderWindow window;
  Grid grid;
  std::unique_ptr<GridRenderer> renderer;
  std::unique_ptr<GameMenu> menu;
  std::unique_ptr<GameOverScreen> gameOverScreen;
  std::unique_ptr<WinScreen> winScreen;

  bool inMenu;
  bool inGameOver;
  bool inWinScreen;

  std::unique_ptr<CustomCursor> customCursor;

public:
  GameManager();
  void run();

  ~GameManager() = default;

private:
  void startGame();
  void resetGame();
};

#endif // OOP_GAMEMANAGER_H