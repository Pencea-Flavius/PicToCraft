#include "GameMenu.h"
#include "Exceptions.h"
#include "MenuResolution.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

GameMenu::GameMenu()
    : menuState(MenuState::MainMenu), selectedSourceMode(SourceMode::File),
      gridSize(5),
      availableResolutions(MenuResolution::getAvailableResolutions()),
      buttonManager(font, buttonTexture, buttonDisabledTexture),
      selectedFileIndex(0), selectedDifficultyIndex(0),
      currentResolutionIndex(0), pendingFullscreen(false),
      initialFullscreenState(false) {

  // Find desktop resolution index
  auto desktop = sf::VideoMode::getDesktopMode();
  for (size_t i = 0; i < availableResolutions.size(); ++i) {
    if (availableResolutions[i].size == desktop.size) {
      currentResolutionIndex = static_cast<int>(i);
      break;
    }
  }

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
    throw GameException("Eroare la citirea folderului nivele: " +
                        std::string(e.what()));
  }

  if (availableFiles.empty()) {
    throw GameException("Nici un fisier gasit in folderul nivele!");
  }

  // Set default file
  // Set default file
  selectedFile = availableFiles[0];

  setupMainMenu();
}

GameMenu::~GameMenu() = default;

void GameMenu::reset() {
  menuState = MenuState::MainMenu;
  setupMainMenu();
}

void GameMenu::loadAssets() {
  (void)font.openFromFile("assets/Monocraft.ttf");
  (void)subtitleFont.openFromFile("assets/MinecraftTen.ttf");

  if (!titleTexture.loadFromFile("assets/pictocraft.png")) {
    throw AssetLoadException("assets/pictocraft.png", "Texture");
  }

  titleSprite = sf::Sprite(titleTexture);
  auto bounds = titleSprite->getLocalBounds();
  titleSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

  if (!buttonTexture.loadFromFile("assets/buttons/button.png")) {
    throw AssetLoadException("assets/buttons/button.png", "Texture");
  }
  if (!buttonDisabledTexture.loadFromFile(
          "assets/buttons/button_disabled.png")) {
    throw AssetLoadException("assets/buttons/button_disabled.png", "Texture");
  }

  if (!menuBackgroundTexture.loadFromFile("assets/menu/menu_background.png")) {
    throw AssetLoadException("assets/menu/menu_background.png", "Texture");
  }
  menuBackgroundTexture.setRepeated(true);

  if (!tabHeaderBackgroundTexture.loadFromFile(
          "assets/menu/tab_header_background.png")) {
    throw AssetLoadException("assets/menu/tab_header_background.png",
                             "Texture");
  }
  tabHeaderBackgroundTexture.setRepeated(true);

  if (!headerSeparatorTexture.loadFromFile(
          "assets/menu/header_separator.png")) {
    throw AssetLoadException("assets/menu/header_separator.png", "Texture");
  }
  headerSeparatorTexture.setRepeated(true);

  if (!footerSeparatorTexture.loadFromFile(
          "assets/menu/footer_separator.png")) {
    throw AssetLoadException("assets/menu/footer_separator.png", "Texture");
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

void GameMenu::setupOptionsScreen() {
  buttonManager.createButtons({}, 20); // Clear buttons

  float sliderVal = 0.0f;
  if (availableResolutions.size() > 1) {
    sliderVal = static_cast<float>(currentResolutionIndex) /
                (availableResolutions.size() - 1);
  }

  std::string resLabel =
      "Resolution: " + MenuResolution::resolutionToString(
                           availableResolutions[currentResolutionIndex]);
  buttonManager.addSlider(resLabel, sliderVal,
                          static_cast<int>(availableResolutions.size()), 20);

  if (pendingFullscreen) {
    buttonManager.setButtonEnabled(0, false);
  }

  buttonManager.addButton(
      pendingFullscreen ? "Fullscreen: ON" : "Fullscreen: OFF", 20);

  buttonManager.addButton("Done", 20);
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
        case MenuState::Options:
          handleOptionsClick(clickedIndex, window);
          break;
        default:
          break;
        }
      }
    }
  } else if (const auto *mouseRelease =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    if (mouseRelease->button == sf::Mouse::Button::Left) {
      buttonManager.stopDrag();
    }
  } else if (const auto *mouseMove = event.getIf<sf::Event::MouseMoved>()) {
    if (menuState == MenuState::Options) {
      sf::Vector2f mousePos = window.mapPixelToCoords(
          {mouseMove->position.x, mouseMove->position.y});
      buttonManager.handleDrag(mousePos);

      if (buttonManager.getButtonCount() > 0) {
        float val = buttonManager.getSliderValue(0);
        int steps = static_cast<int>(availableResolutions.size());
        if (steps > 1) {
          int index = static_cast<int>(std::round(val * (steps - 1)));
          if (index >= 0 && index < steps) {
            currentResolutionIndex = index;
            std::string resLabel =
                "Resolution: " +
                MenuResolution::resolutionToString(
                    availableResolutions[currentResolutionIndex]);
            buttonManager.setButtonText(0, resLabel);
          }
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
    menuState = MenuState::Options;
    initialFullscreenState = pendingFullscreen;
    setupOptionsScreen();
  } else if (buttonIndex == 2) {
    menuState = MenuState::Quitting;
  }
}

void GameMenu::handleOptionsClick(int buttonIndex,
                                  const sf::RenderWindow &window) {
  if (buttonIndex == 0) {
    // Resolution slider - handled in update()
  } else if (buttonIndex == 1) {
    // Fullscreen: toggle
    pendingFullscreen = !pendingFullscreen;
    buttonManager.setButtonText(1, pendingFullscreen ? "Fullscreen: ON"
                                                     : "Fullscreen: OFF");
    buttonManager.setButtonEnabled(0, !pendingFullscreen);
  } else if (buttonIndex == 2) {
    auto selectedRes = availableResolutions[currentResolutionIndex];
    if (selectedRes.size.x != window.getSize().x ||
        selectedRes.size.y != window.getSize().y ||
        pendingFullscreen != initialFullscreenState) {
      pendingResolutionChange = selectedRes;
    }
    menuState = MenuState::MainMenu;
    setupMainMenu();
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

    if (titleSprite) {
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

  case MenuState::Options:
    drawOptions(window);
    break;

  default:
    break;
  }
}

void GameMenu::drawOptions(sf::RenderWindow &window) {
  panorama.draw(window);
  drawOverlay(window);

  auto [scale, scaleY] = calculateScale(window);
  buttonManager.layoutOptions(window, scale, scaleY);
  buttonManager.draw(window);
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
