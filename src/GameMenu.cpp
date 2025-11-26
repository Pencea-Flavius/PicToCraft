#include "GameMenu.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>

GameMenu::GameMenu()
    : fontLoaded(false), subtitleFontLoaded(false), titleLoaded(false),
      buttonLoaded(false), menuState(MenuState::MainMenu),
      selectedSourceMode(SourceMode::File), gridSize(5),
      buttonManager(font, buttonTexture, buttonDisabledTexture),
      selectedFileIndex(0), selectedDifficultyIndex(0) {

  gameConfig.baseMode = GameModeType::Score;
  gameConfig.timeMode = false;
  gameConfig.torchMode = false;

  loadAssets();

  difficultyOptions = {
      {"Peaceful", 5}, {"Normal", 8}, {"Hard", 12}, {"Hardcore", 16}};

  try {
    std::filesystem::path levelDir("nivele");
    if (std::filesystem::exists(levelDir) &&
        std::filesystem::is_directory(levelDir)) {
      for (const auto &entry : std::filesystem::directory_iterator(levelDir)) {
        if (entry.is_regular_file()) {
          std::string filename = entry.path().filename().string();
          if (filename.length() >= 4 &&
              filename.compare(filename.length() - 4, 4, ".txt") == 0) {
            availableFiles.push_back(entry.path().string());
          }
        }
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Eroare la citirea folderului nivele: " << e.what() << "\n";
  }

  if (availableFiles.empty()) {
    std::cerr << "Nici un fisier gasit in folderul nivele!\n";
    availableFiles.emplace_back("nivele/default.txt");
  }

  // Set default file
  if (!availableFiles.empty()) {
    selectedFile = availableFiles[0];
  }

  setupMainMenu();
}

GameMenu::~GameMenu() = default;

void GameMenu::reset() {
  menuState = MenuState::MainMenu;
  setupMainMenu();
}

void GameMenu::loadAssets() {
  fontLoaded = font.openFromFile("assets/Monocraft.ttf");
  subtitleFontLoaded = subtitleFont.openFromFile("assets/MinecraftTen.ttf");

  titleLoaded = titleTexture.loadFromFile("assets/pictocraft.png");
  if (titleLoaded) {
    titleSprite = sf::Sprite(titleTexture);
    auto bounds = titleSprite->getLocalBounds();
    titleSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
  }

  buttonLoaded = buttonTexture.loadFromFile("assets/button.png");
  if (!buttonDisabledTexture.loadFromFile("assets/button_disabled.png")) {
    std::cerr << "Failed to load button_disabled.png\n";
  }

  if (!menuBackgroundTexture.loadFromFile("assets/menu/menu_background.png")) {
    std::cerr << "Failed to load menu_background.png\n";
  }
  menuBackgroundTexture.setRepeated(true);

  if (!tabHeaderBackgroundTexture.loadFromFile(
          "assets/menu/tab_header_background.png")) {
    std::cerr << "Failed to load tab_header_background.png\n";
  }
  tabHeaderBackgroundTexture.setRepeated(true);

  if (!headerSeparatorTexture.loadFromFile(
          "assets/menu/header_separator.png")) {
    std::cerr << "Failed to load header_separator.png\n";
  }
  headerSeparatorTexture.setRepeated(true);

  if (!footerSeparatorTexture.loadFromFile(
          "assets/menu/footer_separator.png")) {
    std::cerr << "Failed to load footer_separator.png\n";
  }
  footerSeparatorTexture.setRepeated(true);
}

void GameMenu::update(float deltaTime) {
  panorama.update(deltaTime);

  if (menuState == MenuState::MainMenu) {
    splashText.update(deltaTime);
  }
}

void GameMenu::setupMainMenu() {
  subtitleText.reset();
  buttonManager.createButtons({"Singleplayer", "Options", "Quit"}, 20);
}

void GameMenu::setupGameSetupScreen() {
  std::vector<std::string> labels;

  labels.emplace_back("Game");
  labels.emplace_back("Modifiers");

  if (selectedTab == 0) {
    labels.emplace_back(selectedSourceMode == SourceMode::Random
                            ? "Source: Random"
                            : "Source: File");

    labels.emplace_back(gameConfig.baseMode == GameModeType::Score
                            ? "Mode: Training"
                            : "Mode: Damage");

    if (selectedSourceMode == SourceMode::Random) {
      labels.push_back("Difficulty: " +
                       difficultyOptions[selectedDifficultyIndex].name);
    } else {
      std::filesystem::path p(selectedFile);
      std::string filename = p.stem().string();
      labels.push_back("File: " + filename);
    }
  } else {
    if (gameConfig.baseMode == GameModeType::Mistakes) {
      labels.emplace_back(gameConfig.timeMode ? "Time: ON" : "Time: OFF");
    } else {
      labels.emplace_back("Time: N/A");
    }

    labels.emplace_back(gameConfig.torchMode ? "Torch: ON" : "Torch: OFF");
  }

  labels.emplace_back("Play Selected Game");
  labels.emplace_back("Cancel");

  buttonManager.createButtons(labels, 20);
}

void GameMenu::handleEvent(const sf::Event &event,
                           const sf::RenderWindow &window) {

  if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseButton->button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = window.mapPixelToCoords(
          {mouseButton->position.x, mouseButton->position.y});

      int clickedIndex = buttonManager.handleClick(mousePos);
      if (clickedIndex >= 0) {
        switch (menuState) {
        case MenuState::MainMenu:
          handleMainMenuClick(clickedIndex);
          break;
        case MenuState::GameSetup:
          handleGameSetupClick(clickedIndex);
          break;
        default:
          break;
        }
      }
    }
  }
}

void GameMenu::handleMainMenuClick(int buttonIndex) {
  if (buttonIndex == 0) {
    menuState = MenuState::GameSetup;
    setupGameSetupScreen();
  } else if (buttonIndex == 1) {
    std::cout << "Options clicked\n";
  } else if (buttonIndex == 2) {
    menuState = MenuState::Quitting;
  }
}

void GameMenu::handleGameSetupClick(int buttonIndex) {
  int playIndex = static_cast<int>(buttonManager.getButtonCount() - 2);
  int cancelIndex = static_cast<int>(buttonManager.getButtonCount() - 1);

  if (buttonIndex == 0) {
    selectedTab = 0;
    setupGameSetupScreen();
    return;
  }
  if (buttonIndex == 1) {
    selectedTab = 1;
    setupGameSetupScreen();
    return;
  }

  if (buttonIndex == playIndex) {
    if (selectedSourceMode == SourceMode::Random) {
      gridSize = difficultyOptions[selectedDifficultyIndex].gridSize;
    }
    menuState = MenuState::Starting;
    return;
  }

  if (buttonIndex == cancelIndex) {
    menuState = MenuState::MainMenu;
    setupMainMenu();
    return;
  }

  if (selectedTab == 0) {
    if (buttonIndex == 2) {
      selectedSourceMode = (selectedSourceMode == SourceMode::File)
                               ? SourceMode::Random
                               : SourceMode::File;
      setupGameSetupScreen();
    } else if (buttonIndex == 3) {
      gameConfig.baseMode = (gameConfig.baseMode == GameModeType::Score)
                                ? GameModeType::Mistakes
                                : GameModeType::Score;
      if (gameConfig.baseMode == GameModeType::Score) {
        gameConfig.timeMode = false;
      }
      setupGameSetupScreen();
    } else if (buttonIndex == 4) {
      if (selectedSourceMode == SourceMode::Random) {
        selectedDifficultyIndex = static_cast<int>(
            (selectedDifficultyIndex + 1) % difficultyOptions.size());
      } else {
        if (!availableFiles.empty()) {
          selectedFileIndex =
              static_cast<int>((selectedFileIndex + 1) % availableFiles.size());
          selectedFile = availableFiles[selectedFileIndex];
        }
      }
      setupGameSetupScreen();
    }
  } else {
    if (buttonIndex == 2) {
      if (gameConfig.baseMode == GameModeType::Mistakes) {
        gameConfig.timeMode = !gameConfig.timeMode;
        setupGameSetupScreen();
      }
    } else if (buttonIndex == 3) {
      gameConfig.torchMode = !gameConfig.torchMode;
      setupGameSetupScreen();
    }
  }
}

void GameMenu::draw(sf::RenderWindow &window) {
  window.clear(sf::Color(40, 40, 40));

  auto [scale, scaleY] = calculateScale(window);

  switch (menuState) {
  case MenuState::MainMenu: {
    panorama.draw(window);

    if (titleLoaded && titleSprite) {
      float logoScale = scale * 0.5f;
      titleSprite->setScale({logoScale, logoScale});
      titleSprite->setPosition(
          {static_cast<float>(window.getSize().x) / 2.0f, 100.0f * scaleY});
      window.draw(*titleSprite);
      splashText.draw(window, *titleSprite, scale, scaleY, logoOriginalWidth,
                      logoOriginalHeight);
    }

    buttonManager.layoutMainMenu(window, scale, scaleY);
    buttonManager.draw(window);
    break;
  }

  case MenuState::GameSetup:
    drawGameSetup(window);
    break;

  default:
    break;
  }
}

void GameMenu::drawGameSetup(sf::RenderWindow &window) {
  panorama.draw(window);
  drawOverlay(window);

  auto [scale, scaleY] = calculateScale(window);

  bool isTimeModeAvailable = (gameConfig.baseMode == GameModeType::Mistakes);
  buttonManager.layoutGameSetup(window, scale, scaleY, selectedTab,
                                isTimeModeAvailable, buttonTexture,
                                buttonDisabledTexture);
  buttonManager.draw(window);
}

void GameMenu::drawOverlay(sf::RenderWindow &window) const {
  auto [scale, scaleY] = calculateScale(window);

  float headerHeight = std::round(100.0f * scaleY);
  float footerHeight = headerHeight;

  // Use integer scaling for separator to avoid tiling artifacts
  float separatorScale = std::floor(scaleY);
  if (separatorScale < 1.0f)
    separatorScale = 1.0f;
  float separatorHeight = 10.0f * separatorScale;

  float middleHeight = window.getSize().y - headerHeight - footerHeight;
  if (middleHeight > 0) {
    sf::RectangleShape overlay(
        {static_cast<float>(window.getSize().x), middleHeight});
    overlay.setPosition({0.f, headerHeight});
    overlay.setFillColor(sf::Color(0, 0, 0, 130));
    window.draw(overlay);
  }
  sf::RectangleShape headerSep(
      {static_cast<float>(window.getSize().x), separatorHeight});
  headerSep.setTexture(&headerSeparatorTexture);
  headerSep.setPosition({0.f, headerHeight});
  window.draw(headerSep);

  sf::RectangleShape footerSep(
      {static_cast<float>(window.getSize().x), separatorHeight});
  footerSep.setTexture(&footerSeparatorTexture);
  footerSep.setPosition(
      {0.f, window.getSize().y - footerHeight - separatorHeight});
  window.draw(footerSep);
}

sf::Vector2f GameMenu::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}
