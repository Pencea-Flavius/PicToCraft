#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include "CustomCursor.h"
#include "GameBackground.h"
#include "GameMenu.h"
#include "GameOverScreen.h"
#include "Grid.h"
#include "GridRenderer.h"
#include "WinScreen.h"
#include "Leaderboard.h"
#include <SFML/Graphics.hpp>
#include <memory>

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

  sf::SoundBuffer deathBuffer;
  sf::Sound deathSound;

  std::unique_ptr<GameBackground> background;
  
  // Audio
  sf::Music alphaMusic;
  sf::Music c418Music;
  std::vector<std::string> c418Tracks;
  float musicPauseTimer;
  float nextPauseDuration;
  
  void updateMusic(float deltaTime);
  
  Leaderboard leaderboard;

public:
  GameManager();
  void run();

  ~GameManager() = default;

private:
  void startGame();
  void resetGame();
};

#endif // OOP_GAMEMANAGER_H