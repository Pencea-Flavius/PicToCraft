#include "GameManager.h"
#include "Exceptions.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <iostream>
#include <optional>

GameManager::GameManager()
    : grid(), inMenu(true), inGameOver(false), inWinScreen(false),
      deathSound(deathBuffer) {

  auto desktop = sf::VideoMode::getDesktopMode();
  unsigned int width = 1280;
  unsigned int height = 720;
  if (desktop.size.x < width)
    width = desktop.size.x;
  if (desktop.size.y < height)
    height = desktop.size.y;

  window.create(sf::VideoMode({width, height}), "PictoCraft", sf::Style::Close,
                sf::State::Windowed);
  window.setFramerateLimit(60);

  menu = std::make_unique<GameMenu>();
  gameOverScreen = std::make_unique<GameOverScreen>();
  winScreen = std::make_unique<WinScreen>();

  bool enableCustomCursor = true;
  float cursorScale = (static_cast<float>(width) / 1920.0f) * 0.2f;
  customCursor = std::make_unique<CustomCursor>(window);
  customCursor->setScale(cursorScale);
  customCursor->setEnabled(enableCustomCursor);
  if (!deathBuffer.loadFromFile("assets/sound/hurt2.mp3")) {
    throw AssetLoadException("assets/sound/hurt2.mp3", "Sound");
  }
}

void GameManager::startGame() {
  GameConfig config = menu->getGameConfig();

  if (menu->getSourceMode() == SourceMode::File) {
    grid.load_from_file(menu->getSelectedFile(), config);
  } else {
    grid.generate_random(menu->getGridSize(), config, 0.6);
  }

  if (customCursor) {
    customCursor->setTorchMode(config.torchMode);
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

  float availableWidth = static_cast<float>(winSize.x) * 0.85f;
  float availableHeight = static_cast<float>(winSize.y) * 0.85f;

  float cellSizeByWidth =
      availableWidth /
      (static_cast<float>(n) + static_cast<float>(maxRowWidth) * 0.8f);
  float cellSizeByHeight =
      availableHeight /
      (static_cast<float>(n) + static_cast<float>(maxColHeight) * 0.8f);
  float cellSize = std::min(cellSizeByWidth, cellSizeByHeight);

  float totalWidth =
      (static_cast<float>(n) + static_cast<float>(maxRowWidth) * 0.8f) *
      cellSize;
  float totalHeight =
      (static_cast<float>(n) + static_cast<float>(maxColHeight) * 0.8f) *
      cellSize;

  float offsetX = (static_cast<float>(winSize.x) - totalWidth) / 2.f;
  float offsetY = (static_cast<float>(winSize.y) - totalHeight) / 2.f;

  renderer = std::make_unique<GridRenderer>(grid, cellSize,
                                            sf::Vector2f(offsetX, offsetY));
  grid.setRenderer(renderer.get());
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
        window.setView(::sf::View(visibleArea));

        if (!inMenu && !inGameOver && !inWinScreen) {
          resetGame();
        }

        if (customCursor) {
          // Base width 1920, base scale 0.2f
          float newScale =
              (static_cast<float>(resized->size.x) / 1920.0f) * 0.2f;
          customCursor->setScale(newScale);
        }
      }
      if (customCursor) {
        customCursor->handleEvent(*event);
      }

      if (inMenu) {
        menu->handleEvent(*event, window);

        if (auto res = menu->getPendingResolutionChange()) {
          if (menu->getPendingFullscreen()) {
            window.create(*res, "PictoCraft", sf::State::Fullscreen);
          } else {
            window.create(*res, "PictoCraft", sf::Style::Close,
                          sf::State::Windowed);
          }
          window.setFramerateLimit(60);
          if (customCursor) {
            float newScale =
                (static_cast<float>(window.getSize().x) / 1920.0f) * 0.2f;
            customCursor->setScale(newScale);
          }
        }

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
          GameConfig config = menu->getGameConfig();
          if (menu->getSourceMode() == SourceMode::File) {
            grid.load_from_file(menu->getSelectedFile(), config);
          } else {
            grid.generate_random(menu->getGridSize(), config,
                                 0.6); // Assuming 0.6 is the default difficulty
          }
          if (customCursor) {
            customCursor->setTorchMode(config.torchMode);
          }
          resetGame();
          inGameOver = false;
        } else if (action == GameOverAction::MainMenu) {
          menu->reset();
          menu->reset();
          inMenu = true;
          inGameOver = false;
          if (customCursor) {
            customCursor->setTorchMode(false);
          }
        }
      } else if (inWinScreen) {
        if (auto key = event->getIf<sf::Event::KeyPressed>()) {
          if (key->code == sf::Keyboard::Key::Enter ||
              key->code == sf::Keyboard::Key::Space) {
            menu->reset();
            inMenu = true;
            inWinScreen = false;
            if (customCursor) {
              customCursor->setTorchMode(false);
            }
          }
        }
      } else {
        if (!grid.handleInput(*event, window)) {
          if (event->is<sf::Event::MouseButtonPressed>()) {
            auto m = event->getIf<sf::Event::MouseButtonPressed>();
            if (m && m->button == sf::Mouse::Button::Left) {
              renderer->handleClick(sf::Mouse::getPosition(window));
            }
          }
        }
      }
    }

    if (inMenu) {
      menu->update(deltaTime);
      menu->draw(window);
    } else if (inWinScreen) {
      window.clear(::sf::Color::Black);
      winScreen->update(deltaTime);
      winScreen->draw(window);
      if (winScreen->isFinished()) {
        menu->reset();
        inMenu = true;
        inWinScreen = false;
        if (customCursor) {
          customCursor->setTorchMode(false);
        }
      }
    } else {
      window.clear(sf::Color(240, 240, 240));
      grid.update(deltaTime);

      if (!inGameOver && !inWinScreen) {
        if (grid.is_solved()) {
          inWinScreen = true;
          winScreen->reset();
          winScreen->setScore(grid.get_score());
        } else if (grid.is_lost()) {
          inGameOver = true;
          gameOverScreen->reset();
          gameOverScreen->setScore(grid.get_score());
          deathSound.play();
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