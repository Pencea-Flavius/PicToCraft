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
    void loadAssets();
    void setupModeSelectionScreen();
    void setupSourceSelectionScreen();
    void setupFileSelectionScreen();
    void setupRandomConfigScreen();

    void drawModeSelection(sf::RenderWindow& window);
    void drawSourceSelection(sf::RenderWindow& window);
    void drawFileSelection(sf::RenderWindow& window);
    void drawRandomConfig(sf::RenderWindow& window);

    void handleModeSelectionClick(const sf::Vector2f& mousePos);
    void handleSourceSelectionClick(const sf::Vector2f& mousePos);
    void handleFileSelectionClick(const sf::Vector2f& mousePos);
    void handleRandomConfigClick(const sf::Vector2f& mousePos);

    // Assets
    sf::Font font;
    sf::Texture titleTexture;
    sf::Texture buttonTexture;
    sf::Texture backgroundTexture;
    bool fontLoaded;
    bool titleLoaded;
    bool buttonLoaded;
    bool backgroundLoaded;

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
    float baseWidth = 1280.f;
    float baseHeight = 720.f;

    // Dimensiuni logo
    float logoOriginalWidth = 1003.f;
    float logoOriginalHeight = 162.f;
};

#endif //OOP_GAMEMENU_H