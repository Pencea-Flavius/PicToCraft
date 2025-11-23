#include "GameManager.h"
#include <iostream>
#include <optional>

GameManager::GameManager()
    : grid(), inMenu(true), inGameOver(false), inWinScreen(false) {
  auto &menuRes = MenuResolution::getInstance();
  sf::VideoMode mode = menuRes.selectResolution();

  if (menuRes.wasFullscreenChosen())
    window.create(mode, "PictoCraft", sf::State::Fullscreen);
  else
    window.create(mode, "PictoCraft", sf::Style::Close, sf::State::Windowed);

  window.setFramerateLimit(60);

  menu = std::make_unique<GameMenu>();
  gameOverScreen = std::make_unique<GameOverScreen>();
  winScreen = std::make_unique<WinScreen>();
}

void GameManager::startGame() {
  GameModeType mode = menu->getGameMode();

  if (menu->getSourceMode() == SourceMode::File) {
    grid.load_from_file(menu->getSelectedFile(), mode);
  } else {
    grid.generate_random(menu->getGridSize(), mode, 0.6);
  }

  resetGame();
  inMenu = false;
  inGameOver = false;
  inWinScreen = false;
}

void GameManager::resetGame() {
  auto winSize = window.getSize();
  int n = grid.get_size();

  const auto &hints = grid.get_hints();
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

  renderer = std::make_unique<GridRenderer>(grid, cellSize,
                                            sf::Vector2f(offsetX, offsetY));
}

void GameManager::run() {
  sf::Clock clock;

  while (window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();

    while (const std::optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
        break;
      }
      if (auto key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
          window.close();
          break;
        }
      }
      if (auto resized = event->getIf<sf::Event::Resized>()) {
        sf::FloatRect visibleArea({0.f, 0.f},
                                  {static_cast<float>(resized->size.x),
                                   static_cast<float>(resized->size.y)});
        window.setView(sf::View(visibleArea));

        if (!inMenu && !inGameOver && !inWinScreen) {
          resetGame();
        }
      }

      if (inMenu) {
        menu->handleEvent(*event, window);

        if (menu->shouldQuit()) {
          window.close();
          break;
        }

        if (menu->isGameReady()) {
          startGame();
        }
      } else if (inGameOver) {
        GameOverAction action = gameOverScreen->handleEvent(*event, window);
        if (action == GameOverAction::Retry) {
          GameModeType mode = menu->getGameMode();
          if (menu->getSourceMode() == SourceMode::File) {
            grid.load_from_file(menu->getSelectedFile(), mode);
          } else {
            grid.generate_random(menu->getGridSize(), mode,
                                 0.6); // Assuming 0.6 is the default difficulty
          }
          resetGame();
          inGameOver = false;
        } else if (action == GameOverAction::MainMenu) {
          menu->reset();
          inMenu = true;
          inGameOver = false;
        }
      } else if (inWinScreen) {
        if (auto key = event->getIf<sf::Event::KeyPressed>()) {
          if (key->code == sf::Keyboard::Key::Enter ||
              key->code == sf::Keyboard::Key::Space) {
            menu->reset();
            inMenu = true;
            inWinScreen = false;
          }
        }
      } else {
        if (event->is<sf::Event::MouseButtonPressed>()) {
          auto m = event->getIf<sf::Event::MouseButtonPressed>();
          if (m && m->button == sf::Mouse::Button::Left) {
            renderer->handleClick(sf::Mouse::getPosition(window));
          }
        }
      }
    }

    if (inMenu) {
      menu->update(deltaTime);
      menu->draw(window);
    } else if (inWinScreen) {
      window.clear(sf::Color::Black);
      winScreen->update(deltaTime);
      winScreen->draw(window);
    } else {
      window.clear(sf::Color(240, 240, 240));
      grid.update(deltaTime);

      if (!inGameOver && !inWinScreen) {
        if (grid.is_solved()) {
          std::cout << "Puzzle rezolvat! Felicitari!\n";
          inWinScreen = true;
          winScreen->reset();
        } else if (grid.is_lost()) {
          std::cout << "Ai pierdut jocul!\n";
          inGameOver = true;
          gameOverScreen->setScore(grid.get_score());
        }
      }
      renderer->draw(window);
      renderer->drawGameInfo(window);

      if (inGameOver) {
        gameOverScreen->update(window);
        gameOverScreen->draw(window);
      }
    }

    window.display();
  }
}