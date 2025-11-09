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

class GameMenu {
public:
    GameMenu();

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

    bool isGameReady() const { return menuState == MenuState::Starting; }
    GameMode getGameMode() const { return selectedGameMode; }
    SourceMode getSourceMode() const { return selectedSourceMode; }
    std::string getSelectedFile() const { return selectedFile; }
    int getGridSize() const { return gridSize; }

private:
    void loadFont();
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

    sf::Font font;
    bool fontLoaded;

    MenuState menuState;
    GameMode selectedGameMode;
    SourceMode selectedSourceMode;
    std::string selectedFile;
    int gridSize;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> subtitleText;

    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    std::vector<sf::RectangleShape> buttonHovers;

    int hoveredButton;

    std::vector<std::string> availableFiles;
    int selectedFileIndex;

    // Random config
    std::vector<int> gridSizeOptions;
    int selectedSizeIndex;
};

#endif //OOP_GAMEMENU_H