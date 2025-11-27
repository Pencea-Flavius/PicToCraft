#ifndef OOP_GAMEMENU_H
#define OOP_GAMEMENU_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "MenuButtonManager.h"
#include "MenuPanorama.h"
#include "SplashText.h"

enum class MenuState { MainMenu, GameSetup, Options, Starting, Quitting };

#include "GameMode.h"

enum class SourceMode { File, Random };

struct DifficultyOption {
  std::string name;
  int gridSize;
};

class GameMenu {
public:
  GameMenu();
  ~GameMenu();

  void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
  void update(float deltaTime);
  void draw(sf::RenderWindow &window);

  bool isGameReady() const { return menuState == MenuState::Starting; }
  bool shouldQuit() const { return menuState == MenuState::Quitting; }

  GameConfig getGameConfig() const { return gameConfig; }
  SourceMode getSourceMode() const { return selectedSourceMode; }
  const std::string &getSelectedFile() const { return selectedFile; }
  int getGridSize() const { return gridSize; }
  void reset();

  // Resolution handling
  std::optional<sf::VideoMode> getPendingResolutionChange() {
    auto res = pendingResolutionChange;
    pendingResolutionChange.reset();
    return res;
  }
  bool getPendingFullscreen() const { return pendingFullscreen; }

private:
  // Asset loading
  void loadAssets();

  // Screen setup functions
  void setupMainMenu();
  void setupGameSetupScreen();
  void setupOptionsScreen();

  // Click handlers
  void handleMainMenuClick(int buttonIndex);
  void handleGameSetupClick(int buttonIndex);
  void handleOptionsClick(int buttonIndex, const sf::RenderWindow &window);

  // Helper functions
  sf::Vector2f calculateScale(const sf::RenderWindow &window) const;

  // New drawing helpers
  void drawOverlay(sf::RenderWindow &window) const;
  void drawGameSetup(sf::RenderWindow &window);
  void drawOptions(sf::RenderWindow &window);

  // State
  MenuState menuState;
  GameConfig gameConfig;
  SourceMode selectedSourceMode;
  std::string selectedFile;
  int gridSize;

  // Resolution State
  std::vector<sf::VideoMode> availableResolutions;
  int currentResolutionIndex;
  std::optional<sf::VideoMode> pendingResolutionChange;
  bool pendingFullscreen;
  bool initialFullscreenState;

  // Assets
  sf::Font font;
  sf::Font subtitleFont;
  sf::Texture titleTexture;
  sf::Texture buttonTexture;
  sf::Texture buttonDisabledTexture;

  // New textures
  sf::Texture menuBackgroundTexture;
  sf::Texture menuListBackgroundTexture;
  sf::Texture tabHeaderBackgroundTexture;
  sf::Texture headerSeparatorTexture;
  sf::Texture footerSeparatorTexture;

  // Components
  MenuPanorama panorama;
  SplashText splashText;
  MenuButtonManager buttonManager;

  // UI State for Game Setup
  int selectedTab = 0; // 0 = Game, 1 = Modifiers

  // UI Elements
  std::optional<sf::Sprite> titleSprite;
  std::optional<sf::Text> subtitleText;

  // Available files
  std::vector<std::string> availableFiles;
  int selectedFileIndex;

  // Difficulty options
  std::vector<DifficultyOption> difficultyOptions;
  int selectedDifficultyIndex;

  // Pentru scalare
  float baseWidth = 1280.0f;
  float baseHeight = 720.0f;

  // Dimensiuni logo
  float logoOriginalWidth = 1003.0f;
  float logoOriginalHeight = 162.0f;
};

#endif // OOP_GAMEMENU_H