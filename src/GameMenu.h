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
    Starting
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

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

    bool isGameReady() const { return menuState == MenuState::Starting; }
    GameMode getGameMode() const { return selectedGameMode; }
    SourceMode getSourceMode() const { return selectedSourceMode; }
    const std::string& getSelectedFile() const { return selectedFile; }
    int getGridSize() const { return gridSize; }

private:
    // Asset loading
    void loadAssets();

    // Screen setup functions
    void setupModeSelectionScreen();
    void setupSourceSelectionScreen();
    void setupFileSelectionScreen();
    void setupRandomConfigScreen();

    // Drawing functions
    void drawModeSelection(sf::RenderWindow& window);
    void drawSourceSelection(sf::RenderWindow& window);
    void drawFileSelection(sf::RenderWindow& window);
    void drawRandomConfig(sf::RenderWindow& window);

    // Click handlers
    void handleModeSelectionClick(const sf::Vector2f& mousePos);
    void handleSourceSelectionClick(const sf::Vector2f& mousePos);
    void handleFileSelectionClick(const sf::Vector2f& mousePos);
    void handleRandomConfigClick(const sf::Vector2f& mousePos);

    // Helper functions
    void createButtons(const std::vector<std::string>& labels, unsigned int fontSize);
    void drawBackground(sf::RenderWindow& window);
    sf::Vector2f calculateScale(const sf::RenderWindow& window) const;
    void drawButtons(sf::RenderWindow& window, float startY, float spacing,
                     float buttonScaleX, float buttonScaleY, float textSize);

    // Assets
    sf::Font font;
    sf::Texture titleTexture;
    sf::Texture buttonTexture;
    sf::Texture backgroundTexture;
    bool fontLoaded;
    bool titleLoaded;
    bool buttonLoaded;
    bool backgroundLoaded;

    // State
    MenuState menuState;
    GameMode selectedGameMode;
    SourceMode selectedSourceMode;
    std::string selectedFile;
    int gridSize;

    // UI Elements
    std::optional<sf::Sprite> titleSprite;
    std::optional<sf::Sprite> backgroundSprite;
    std::optional<sf::Text> subtitleText;

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

    // Pentru scalare
    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;

    // Dimensiuni logo
    float logoOriginalWidth = 1003.0f;
    float logoOriginalHeight = 162.0f;
};

#endif //OOP_GAMEMENU_H