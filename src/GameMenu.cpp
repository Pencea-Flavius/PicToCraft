#include "GameMenu.h"
#include "ShadowedText.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

GameMenu::GameMenu()
    : fontLoaded(false), subtitleFontLoaded(false), titleLoaded(false),
      buttonLoaded(false), menuState(MenuState::ModeSelection),
      selectedGameMode(GameModeType::Score),
      selectedSourceMode(SourceMode::File), gridSize(5), selectedFileIndex(0),
      selectedDifficultyIndex(0) {
  loadAssets();

  difficultyOptions = {
      {"Peaceful", 5}, {"Normal", 8}, {"Hard", 12}, {"Hardcore", 16}};

  // Citește fișierele din folderul nivele
  try {
    std::filesystem::path levelDir("nivele");
    if (std::filesystem::exists(levelDir) &&
        std::filesystem::is_directory(levelDir)) {
      for (const auto &entry : std::filesystem::directory_iterator(levelDir)) {
        if (entry.is_regular_file()) {
          std::string filename = entry.path().filename().string();
          if (filename.ends_with(".txt")) {
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

  setupModeSelectionScreen();
}

GameMenu::~GameMenu() = default;

void GameMenu::reset() {
  menuState = MenuState::ModeSelection;
  setupModeSelectionScreen();
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
}

void GameMenu::update(float deltaTime) {
  panorama.update(deltaTime);

  if (menuState == MenuState::ModeSelection) {
    splashText.update(deltaTime);
  }
}

void GameMenu::createButtons(const std::vector<std::string> &labels,
                             unsigned int fontSize) {
  buttons.clear();

  if (!buttonLoaded || !fontLoaded)
    return;

  for (const auto &label : labels) {
    buttons.push_back(
        std::make_unique<MenuButton>(label, font, buttonTexture, fontSize));
  }
}

void GameMenu::createSubtitle(const std::string &text) {
  if (subtitleFontLoaded) {
    subtitleText = sf::Text(subtitleFont, text);
    subtitleText->setCharacterSize(32);
    subtitleText->setFillColor(sf::Color(220, 220, 220));
  }
}

void GameMenu::setupModeSelectionScreen() {
  subtitleText.reset();
  createButtons({"Score Mode", "Mistakes Mode", "Quit"}, 20);
}

void GameMenu::setupSourceSelectionScreen() {
  if (selectedGameMode == GameModeType::Score) {
    createSubtitle("Score Mode");
  } else {
    createSubtitle("Mistakes Mode");
  }
  createButtons({"Play from File", "Random Game", "Back"}, 20);
}

void GameMenu::setupFileSelectionScreen() {
  createSubtitle("Select Level");

  std::vector<std::string> fileNames;
  for (const auto &filePath : availableFiles) {
    std::filesystem::path p(filePath);
    fileNames.push_back(p.stem().string());
  }
  fileNames.emplace_back("Back");

  createButtons(fileNames, 18);
}

void GameMenu::setupRandomConfigScreen() {
  createSubtitle("Select Difficulty");

  std::vector<std::string> difficultyNames;
  for (const auto &difficulty : difficultyOptions) {
    difficultyNames.push_back(difficulty.name);
  }
  difficultyNames.emplace_back("Back");

  createButtons(difficultyNames, 20);
}

void GameMenu::handleEvent(const sf::Event &event,
                           const sf::RenderWindow &window) {

  if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseButton->button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = window.mapPixelToCoords(
          {mouseButton->position.x, mouseButton->position.y});

      switch (menuState) {
      case MenuState::ModeSelection:
        handleModeSelectionClick(mousePos);
        break;
      case MenuState::SourceSelection:
        handleSourceSelectionClick(mousePos);
        break;
      case MenuState::FileSelection:
        handleFileSelectionClick(mousePos);
        break;
      case MenuState::RandomConfig:
        handleRandomConfigClick(mousePos);
        break;
      default:
        break;
      }
    }
  }
}

void GameMenu::handleModeSelectionClick(const sf::Vector2f &mousePos) {
  if (buttons[0]->isClicked(mousePos)) {
    selectedGameMode = GameModeType::Score;
    menuState = MenuState::SourceSelection;
    setupSourceSelectionScreen();
  } else if (buttons[1]->isClicked(mousePos)) {
    selectedGameMode = GameModeType::Mistakes;
    menuState = MenuState::SourceSelection;
    setupSourceSelectionScreen();
  } else if (buttons[2]->isClicked(mousePos)) {
    menuState = MenuState::Quitting;
  }
}

void GameMenu::handleSourceSelectionClick(const sf::Vector2f &mousePos) {
  for (size_t i = 0; i < buttons.size(); i++) {
    if (buttons[i]->isClicked(mousePos)) {
      if (i == 2) {
        menuState = MenuState::ModeSelection;
        setupModeSelectionScreen();
      } else {
        selectedSourceMode = (i == 0) ? SourceMode::File : SourceMode::Random;

        if (selectedSourceMode == SourceMode::File) {
          menuState = MenuState::FileSelection;
          setupFileSelectionScreen();
        } else {
          menuState = MenuState::RandomConfig;
          setupRandomConfigScreen();
        }
      }
      break;
    }
  }
}

void GameMenu::handleFileSelectionClick(const sf::Vector2f &mousePos) {
  for (size_t i = 0; i < buttons.size(); i++) {
    if (buttons[i]->isClicked(mousePos)) {
      if (i == buttons.size() - 1) {
        menuState = MenuState::SourceSelection;
        setupSourceSelectionScreen();
      } else {
        selectedFile = availableFiles[i];
        menuState = MenuState::Starting;
      }
      break;
    }
  }
}

void GameMenu::handleRandomConfigClick(const sf::Vector2f &mousePos) {
  for (size_t i = 0; i < buttons.size(); i++) {
    if (buttons[i]->isClicked(mousePos)) {
      if (i == buttons.size() - 1) {
        menuState = MenuState::SourceSelection;
        setupSourceSelectionScreen();
      } else {
        gridSize = difficultyOptions[i].gridSize;
        menuState = MenuState::Starting;
      }
      break;
    }
  }
}

void GameMenu::draw(sf::RenderWindow &window) {
  window.clear(sf::Color(40, 40, 40));

  // Deseneaza panorama pe toate ecranele
  panorama.draw(window);

  auto [scale, scaleY] = calculateScale(window);

  // Ecrane
  switch (menuState) {
  case MenuState::ModeSelection:
    if (titleLoaded && titleSprite) {
      float logoScale = scale * 0.5f;
      titleSprite->setScale({logoScale, logoScale});
      titleSprite->setPosition(
          {static_cast<float>(window.getSize().x) / 2.0f, 100.0f * scaleY});
      window.draw(*titleSprite);

      splashText.draw(window, *titleSprite, scale, scaleY, logoOriginalWidth,
                      logoOriginalHeight);
    }
    drawButtons(window, 280.0f * scaleY, 80.0f * scaleY, 1.2f, 1.2f);
    break;

  case MenuState::SourceSelection:
    drawSubtitle(window, 80.0f);
    drawButtons(window, 200.0f * scaleY, 80.0f * scaleY, 1.2f, 1.2f);
    break;

  case MenuState::FileSelection:
    drawSubtitle(window, 60.0f);
    drawButtons(window, 160.0f * scaleY, 70.0f * scaleY, 1.4f, 1.0f);
    break;

  case MenuState::RandomConfig:
    drawSubtitle(window, 80.0f);
    drawButtons(window, 200.0f * scaleY, 85.0f * scaleY, 1.2f, 1.0f);
    break;

  default:
    break;
  }
}

sf::Vector2f GameMenu::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}

void GameMenu::drawSubtitle(sf::RenderWindow &window, float yPosition) {
  if (subtitleFontLoaded && subtitleText) {
    auto [scale, scaleY] = calculateScale(window);
    subtitleText->setCharacterSize(static_cast<unsigned int>(36.0f * scale));
    auto subtitleBounds = subtitleText->getLocalBounds();
    subtitleText->setPosition({static_cast<float>(window.getSize().x) / 2.0f -
                                   subtitleBounds.size.x / 2.0f -
                                   subtitleBounds.position.x,
                               yPosition * scaleY});
    ShadowedText::draw(window, *subtitleText, scale);
  }
}

void GameMenu::drawButtons(sf::RenderWindow &window, float startY,
                           float spacing, float buttonScaleX,
                           float buttonScaleY) {
  auto [scale, scaleY] = calculateScale(window);

  sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
  sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

  for (size_t i = 0; i < buttons.size(); i++) {
    float x = static_cast<float>(window.getSize().x) / 2.0f;
    float y = startY + static_cast<float>(i) * spacing;

    buttons[i]->update(window, scale, x, y, buttonScaleX, buttonScaleY,
                       mousePos);
    buttons[i]->draw(window);
  }
}