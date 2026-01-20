#include "GameManager.h"
#include "Exceptions.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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
  if (!window.isOpen()) {
    throw WindowCreationException("Failed to create game window");
  }
  window.setFramerateLimit(60);

  menu = std::make_unique<GameMenu>();
  gameOverScreen = std::make_unique<GameOverScreen>();
  winScreen = std::make_unique<WinScreen>();
  winScreen = std::make_unique<WinScreen>();
  
  try {
      pauseMenu = std::make_unique<PauseMenu>();
  } catch (const std::exception& e) {
      throw AssetLoadException("PauseMenu resources", "PauseMenu");
  }

  isPaused = false;
  
  isPaused = false;

  bool enableCustomCursor = true;
  customCursor = std::make_unique<CustomCursor>(window);
  customCursor->setEnabled(enableCustomCursor);
  if (!deathBuffer.loadFromFile("assets/sound/hurt2.mp3")) {
    throw AssetLoadException("assets/sound/hurt2.mp3", "Sound");
  }

  background = std::make_unique<GameBackground>();

  if (!alphaMusic.openFromFile("assets/sound/Alpha.mp3")) {
    throw AssetLoadException("assets/sound/Alpha.mp3", "Music");
  }
  alphaMusic.setLooping(true);

  c418Tracks = {
      "assets/sound/C418  - Sweden - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Dry Hands - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Haggstrom - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Mice on Venus - Minecraft Volume Alpha.mp3",
      "assets/sound/C418 - Wet Hands - Minecraft Volume Alpha.mp3"};

  musicPauseTimer = 0.0f;
  nextPauseDuration = 0.0f;
  
  try {
    leaderboard.load("leaderboard.txt");
  } catch (const LeaderboardException &e) {
    std::cerr << "Info: Could not load leaderboard: " << e.what() << "\n";
  }
}

GameManager::~GameManager() {
  // Reset cursor to system default before destroying window
  if (customCursor) {
    customCursor->setEnabled(false); // This sets system cursor
  }
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
          std::uniform_int_distribution<> dis(0, static_cast<int>(c418Tracks.size()) - 1);

          if (!c418Tracks.empty()) {
            int idx = dis(gen);
            if (c418Music.openFromFile(c418Tracks[idx])) {
              c418Music.play();
            } else {
              throw AssetLoadException(c418Tracks[idx], "Music");
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
  if (!menu) {
    throw GameStateException("Menu is not initialized");
  }
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

  float availableWidth = static_cast<float>(winSize.x) * 0.95f;
  float availableHeight = static_cast<float>(winSize.y) * 0.95f;

  float cellSizeByWidth =
      availableWidth /
      (static_cast<float>(n) + static_cast<float>(maxRowWidth) * 0.8f * 2.0f);
  float cellSizeByHeight =
      availableHeight /
      (static_cast<float>(n) + static_cast<float>(maxColHeight) * 0.8f * 2.0f);
  float cellSize = std::min(cellSizeByWidth, cellSizeByHeight);

  float gridScreenSize = static_cast<float>(n) * cellSize;
  float rowHintsWidth = static_cast<float>(maxRowWidth) * cellSize * 0.8f;
  float colHintsHeight = static_cast<float>(maxColHeight) * cellSize * 0.8f;

  float offsetX = (static_cast<float>(winSize.x) - gridScreenSize) / 2.f - rowHintsWidth;
  float offsetY = (static_cast<float>(winSize.y) - gridScreenSize) / 2.f - colHintsHeight + 25.0f; // Consistent offset for all modes

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
    // Inside loop
          if (auto key = event->getIf<sf::Event::KeyPressed>()) {
              if (isPaused) {
              } else if (!inMenu && !inGameOver && !inWinScreen) {
                   if (key->code == sf::Keyboard::Key::Escape) {
                       isPaused = true; 
                       continue;
                   }
              }
          }

      if (customCursor) {
        customCursor->handleEvent(*event);
      }
      
      if (isPaused) {
           PauseAction action = pauseMenu->handleEvent(*event, window);
           if (action == PauseAction::Resume) {
               isPaused = false;
               // Reset cursor to center to prevent Alchemy effect cursor jumps
               sf::Vector2i center(window.getSize().x / 2, window.getSize().y / 2);
               sf::Mouse::setPosition(center, window);
           } else if (action == PauseAction::MainMenu) {
               isPaused = false;
               menu->reset();
               inMenu = true;
               if (customCursor) customCursor->setTorchMode(false);
           }
           continue; // Skip other updates
      }

      if (inMenu) {

        menu->handleEvent(*event, window);

        if (auto res = menu->getPendingResolutionChange()) {
          if (menu->getPendingFullscreen()) {
            // Use desktop mode for fullscreen to prevent changing monitor resolution
            window.create(sf::VideoMode::getDesktopMode(), "PictoCraft", sf::State::Fullscreen);
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
          if (key->code == sf::Keyboard::Key::Enter) {
            // Enter skips everything
            menu->reset();
            inMenu = true;
            inWinScreen = false;
            if (customCursor) {
              customCursor->setTorchMode(false);
            }
          } else if (key->code == sf::Keyboard::Key::Space) {
            // Space speeds up scrolling
            winScreen->setSpeedMultiplier(3.0f);
          }
        } else if (auto keyReleased = event->getIf<sf::Event::KeyReleased>()) {
          if (keyReleased->code == sf::Keyboard::Key::Space) {
            // Return to normal speed when Space is released
            winScreen->setSpeedMultiplier(1.0f);
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
      
      if (!isPaused && !inGameOver && !inWinScreen) {
          grid.update(deltaTime);
      }

      if (!inGameOver && !inWinScreen) {
        if (!isPaused && grid.is_solved()) { // Guard win check
          inWinScreen = true;
          winScreen->reset();
          
          int baseScore = grid.get_score();
          GameConfig cfg = menu->getGameConfig();
          int bonus = cfg.calculateBonus();
          
          int finalScore = baseScore + bonus;
          
          leaderboard.addEntry(menu->getPlayerName(), finalScore);
          try {
            leaderboard.save("leaderboard.txt");
          } catch (const LeaderboardException &e) {
            std::cerr << "Warning: Failed to save leaderboard: " << e.what() << "\n";
          }
          
          winScreen->setScore(finalScore, leaderboard);
        } else if (!isPaused && grid.is_lost()) { // Guard loss check
          inGameOver = true;
          gameOverScreen->reset();
          gameOverScreen->setScore(grid.get_score());
          deathSound.play();
        }
      }
      
      if (background) {
        GameConfig cfg = menu->getGameConfig();
        bool shouldScroll = cfg.backgroundMovement;
        if (cfg.discoFeverMode) shouldScroll = true; 
        
        if (!isPaused && !inGameOver && !inWinScreen) {
            background->update(deltaTime, window.getSize(), shouldScroll);
        }
        background->draw(window);
      }
      renderer->draw(window);
      renderer->drawGameInfo(window);

      if (inGameOver) {
        gameOverScreen->update(window);
        gameOverScreen->draw(window);
      }
      
      if (isPaused) {
          pauseMenu->update(window);
          pauseMenu->draw(window);
      }
    }

    updateMusic(deltaTime);

    // Apply volumes
    GameConfig cfg = menu->getGameConfig();
    sf::Listener::setGlobalVolume(cfg.masterVolume * 100.0f);

    alphaMusic.setVolume(cfg.musicVolume * 100.0f);
    c418Music.setVolume(cfg.musicVolume * 100.0f);

    deathSound.setVolume(cfg.sfxVolume * 100.0f);
    if (background)
      background->setVolume(cfg.sfxVolume * 100.0f);
    grid.setSfxVolume(cfg.sfxVolume * 100.0f);

    // Draw custom cursor debug
    if (customCursor) {
      customCursor->drawDebug(window);
    }

    window.display();
  }
}