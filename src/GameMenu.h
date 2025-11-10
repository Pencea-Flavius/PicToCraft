#ifndef OOP_GAMEMENU_H
#define OOP_GAMEMENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>

enum class MenuState {
    ModeSelection,
    SourceSelection,
    FileSelection,
    RandomConfig,
    Starting,
    Quitting
};

enum class GameMode {
    Score,
    Mistakes
};

enum class SourceMode {
    File,
    Random
};

struct DifficultyOption {
    std::string name;
    int gridSize;
};

class GameMenu {
public:
    GameMenu();
    ~GameMenu();

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isGameReady() const { return menuState == MenuState::Starting; }
    bool shouldQuit() const { return menuState == MenuState::Quitting; }
    GameMode getGameMode() const { return selectedGameMode; }
    SourceMode getSourceMode() const { return selectedSourceMode; }
    const std::string& getSelectedFile() const { return selectedFile; }
    int getGridSize() const { return gridSize; }

private:
    // Asset loading
    void loadAssets();
    void loadSplashMessages();  // NOU

    // Screen setup functions
    void setupModeSelectionScreen();
    void setupSourceSelectionScreen();
    void setupFileSelectionScreen();
    void setupRandomConfigScreen();

    // Drawing functions
    void drawPanorama(sf::RenderWindow& window);
    void drawSplashText(sf::RenderWindow& window);  // NOU

    // Click handlers
    void handleModeSelectionClick(const sf::Vector2f& mousePos);
    void handleSourceSelectionClick(const sf::Vector2f& mousePos);
    void handleFileSelectionClick(const sf::Vector2f& mousePos);
    void handleRandomConfigClick(const sf::Vector2f& mousePos);

    // Helper functions
    void createButtons(const std::vector<std::string>& labels, unsigned int fontSize);
    void createSubtitle(const std::string& text);
    void updateSplashText(float deltaTime);  // NOU
    sf::Vector2f calculateScale(const sf::RenderWindow& window) const;
    void drawButtons(sf::RenderWindow& window, float startY, float spacing,
                     float buttonScaleX, float buttonScaleY, float textSize);
    void drawSubtitle(sf::RenderWindow& window, float yPosition);

    // Assets
    sf::Font font;
    sf::Font subtitleFont;
    sf::Texture titleTexture;
    sf::Texture buttonTexture;
    sf::Texture panoramaTexture;
    bool fontLoaded;
    bool subtitleFontLoaded;
    bool titleLoaded;
    bool buttonLoaded;
    bool panoramaLoaded;

    // State
    MenuState menuState;
    GameMode selectedGameMode;
    SourceMode selectedSourceMode;
    std::string selectedFile;
    int gridSize;

    // UI Elements
    std::optional<sf::Sprite> titleSprite;
    std::optional<sf::Sprite> panoramaSprite1;
    std::optional<sf::Sprite> panoramaSprite2;
    std::optional<sf::Text> subtitleText;
    std::optional<sf::Text> splashText;  // NOU

    // Buttons
    std::vector<sf::Sprite> buttonSprites;
    std::vector<sf::Text> buttonTexts;
    int hoveredButton;

    // Available files
    std::vector<std::string> availableFiles;
    int selectedFileIndex;

    // Difficulty options
    std::vector<DifficultyOption> difficultyOptions;
    int selectedDifficultyIndex;

    // Panorama animation
    float panoramaOffset;
    float panoramaSpeed;

    // Splash text animation  // NOU - tot blocul
    std::vector<std::string> splashMessages;
    bool splashIncreasing;
    float splashScale;
    float splashSpeed;

    // Pentru scalare
    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;

    // Dimensiuni logo
    float logoOriginalWidth = 1003.0f;
    float logoOriginalHeight = 162.0f;
};

#endif //OOP_GAMEMENU_H