#include "GameManager.h"
#include "Exceptions.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <random>

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
  customCursor = std::make_unique<CustomCursor>(window);
  customCursor->setEnabled(enableCustomCursor);
  if (!deathBuffer.loadFromFile("assets/sound/hurt2.mp3")) {
    throw AssetLoadException("assets/sound/hurt2.mp3", "Sound");
  }

  background = std::make_unique<GameBackground>();
  
  if (!alphaMusic.openFromFile("assets/sound/Alpha.mp3")) {
  }
  alphaMusic.setLooping(true);
  
  c418Tracks = {
      "assets/sound/C418  - Sweden - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Dry Hands - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Haggstrom - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Mice on Venus - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Wet Hands - Minecraft Volume Alpha.mp3"
  };
  
  musicPauseTimer = 0.0f;
  nextPauseDuration = 0.0f;
}

void GameManager::updateMusic(float deltaTime) {
    if (inWinScreen) {
        if (c418Music.getStatus() == sf::Music::Status::Playing) {
            c418Music.stop();
        }
        
        if (alphaMusic.getStatus() != sf::Music::Status::Playing) {
            alphaMusic.play();
        }
    } else {
        if (alphaMusic.getStatus() == sf::Music::Status::Playing) {
            alphaMusic.stop();
        }
        
        if (!inMenu && !inGameOver) {
            if (c418Music.getStatus() == sf::Music::Status::Stopped) {
                musicPauseTimer += deltaTime;
                if (musicPauseTimer >= nextPauseDuration) {
                    // Play random track
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, c418Tracks.size() - 1);
                    
                    if (!c418Tracks.empty()) {
                        int idx = dis(gen);
                        if (c418Music.openFromFile(c418Tracks[idx])) {
                             c418Music.play();
                        }
                    }
                    
                    musicPauseTimer = 0.0f;
                    std::uniform_real_distribution<float> timeDis(20.0f, 30.0f);
                    nextPauseDuration = timeDis(gen);
                }
            }
        } else {
             if (c418Music.getStatus() == sf::Music::Status::Playing) {
                 c418Music.pause(); 
             }
        }
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

  if (background) {
    background->selectBackground(config);
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

    if (customCursor) {
      customCursor->update(deltaTime);
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
      if (background) {
        background->update(deltaTime);
        background->draw(window);
      }
      renderer->draw(window);
      renderer->drawGameInfo(window);

      if (inGameOver) {
        gameOverScreen->update(window);
        gameOverScreen->draw(window);
      }
    }
    
    updateMusic(deltaTime);

    // Apply volumes
    GameConfig cfg = menu->getGameConfig();
    sf::Listener::setGlobalVolume(cfg.masterVolume * 100.0f);
    
    alphaMusic.setVolume(cfg.musicVolume * 100.0f);
    c418Music.setVolume(cfg.musicVolume * 100.0f);
    
    deathSound.setVolume(cfg.sfxVolume * 100.0f);
    if (background) background->setVolume(cfg.sfxVolume * 100.0f);
    grid.setSfxVolume(cfg.sfxVolume * 100.0f);

    // Draw custom cursor debug
    if (customCursor) {
      customCursor->drawDebug(window);
    }

    window.display();
  }
}