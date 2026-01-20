#ifndef OOP_GAMEMENU_H
#define OOP_GAMEMENU_H

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

#include "MenuButtonManager.h"
#include "MenuPanorama.h"
#include "SplashText.h"
#include "Leaderboard.h"


enum class MenuState { MainMenu, Highscores, GameSetup, Options, Starting, Quitting };

#include "GameConfig.h"

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

  const GameConfig &getGameConfig() const { return gameConfig; }
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
  void setupHighscoresScreen();
  void setupGameSetupScreen();
  void setupOptionsScreen();

  // Click handlers
  void handleMainMenuClick(int buttonIndex);
  void handleHighscoresClick(int buttonIndex);
  void handleGameSetupClick(int buttonIndex);
  void handleOptionsClick(int buttonIndex, const sf::RenderWindow &window);

  // Helper functions
  sf::Vector2f calculateScale(const sf::RenderWindow &window) const;

  // New drawing helpers
  void drawOverlay(sf::RenderWindow &window) const;
  void drawHighscores(sf::RenderWindow &window);
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
  bool pendingBetaStyle;

  // Assets
  sf::Font font;
  sf::Texture titleTexture;
  sf::Texture buttonTexture;
  sf::Texture buttonDisabledTexture;

  // New textures
  sf::Texture menuBackgroundTexture;
  sf::Texture menuListBackgroundTexture;
  sf::Texture tabHeaderBackgroundTexture;
  sf::Texture headerSeparatorTexture;
  sf::Texture footerSeparatorTexture;

  // Beta assets
  sf::Texture betaBackgroundTexture;
  sf::Texture betaLogoTexture;

  std::optional<sf::Sprite> betaBackgroundSprite;
  std::optional<sf::Sprite> betaLogoSprite;
  
  void drawMenuBackground(sf::RenderWindow &window);

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

  // For Scale
  float baseWidth = 1280.0f;
  float baseHeight = 720.0f;

  // Logo
  float logoOriginalWidth = 1003.0f;
  float logoOriginalHeight = 162.0f;

  // Player Name & Leaderboard
  std::string playerName = "Player";
  Leaderboard leaderboard;
  bool isTypingName = false;

public:
  const std::string& getPlayerName() const { return playerName; }
  void handleTextInput(const sf::Event& event);
};

#endif // OOP_GAMEMENU_H