#include "GameMenu.h"
#include <filesystem>
#include <algorithm>

GameMenu::GameMenu()
    : fontLoaded(false),
      menuState(MenuState::ModeSelection),
      selectedGameMode(GameMode::Mistakes),
      selectedSourceMode(SourceMode::File),
      selectedFile(""),
      gridSize(10),
      hoveredButton(-1),
      selectedFileIndex(0),
      selectedSizeIndex(1)
{
    loadFont();

    // Grid size options
    gridSizeOptions = {5, 10, 15, 20, 25};

    // Scan for available files
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".txt") && filename != "CMakeLists.txt") {
                    availableFiles.push_back(filename);
                }
            }
        }
    } catch (...) {
        // Fallback files
        availableFiles = {"mob.txt", "item.txt", "puzzle.txt"};
    }

    if (availableFiles.empty()) {
        availableFiles.push_back("puzzle.txt");
    }

    setupModeSelectionScreen();
}

void GameMenu::loadFont() {
    fontLoaded = font.openFromFile("assets/Monocraft.ttf");
}

void GameMenu::setupModeSelectionScreen() {
    buttons.clear();
    buttonTexts.clear();
    buttonHovers.clear();

    // Title
    if (fontLoaded) {
        titleText = sf::Text(font, "PICTOCROSS");
        titleText->setCharacterSize(72);
        titleText->setFillColor(sf::Color::White);
        titleText->setOutlineThickness(3);
        titleText->setOutlineColor(sf::Color::Black);

        subtitleText = sf::Text(font, "Alege modul de joc");
        subtitleText->setCharacterSize(28);
        subtitleText->setFillColor(sf::Color(200, 200, 200));
    }

    // Create buttons
    std::vector<std::string> labels = {"Mod Scor", "Mod Greseli"};

    for (size_t i = 0; i < labels.size(); i++) {
        sf::RectangleShape button({400.f, 60.f});
        button.setFillColor(sf::Color(80, 80, 80, 200));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(150, 150, 150));
        buttons.push_back(button);

        sf::RectangleShape hover({400.f, 60.f});
        hover.setFillColor(sf::Color(120, 120, 255, 100));
        buttonHovers.push_back(hover);

        if (fontLoaded) {
            sf::Text text(font, labels[i]);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupSourceSelectionScreen() {
    buttons.clear();
    buttonTexts.clear();
    buttonHovers.clear();

    if (fontLoaded && titleText) {
        titleText->setString("Selecteaza sursa");
        subtitleText->setString(selectedGameMode == GameMode::Score ?
            "Mod: Scor" : "Mod: Greseli (3 maxim)");
    }

    std::vector<std::string> labels = {"Joc din Fisier", "Joc Random"};

    for (size_t i = 0; i < labels.size(); i++) {
        sf::RectangleShape button({400.f, 60.f});
        button.setFillColor(sf::Color(80, 80, 80, 200));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(150, 150, 150));
        buttons.push_back(button);

        sf::RectangleShape hover({400.f, 60.f});
        hover.setFillColor(sf::Color(120, 120, 255, 100));
        buttonHovers.push_back(hover);

        if (fontLoaded) {
            sf::Text text(font, labels[i]);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupFileSelectionScreen() {
    buttons.clear();
    buttonTexts.clear();
    buttonHovers.clear();

    if (fontLoaded && titleText) {
        titleText->setString("Alege fisierul");
        subtitleText->setString("Fisiere disponibile");
    }

    for (const auto& file : availableFiles) {
        sf::RectangleShape button({500.f, 50.f});
        button.setFillColor(sf::Color(80, 80, 80, 200));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(150, 150, 150));
        buttons.push_back(button);

        sf::RectangleShape hover({500.f, 50.f});
        hover.setFillColor(sf::Color(120, 255, 120, 100));
        buttonHovers.push_back(hover);

        if (fontLoaded) {
            sf::Text text(font, file);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupRandomConfigScreen() {
    buttons.clear();
    buttonTexts.clear();
    buttonHovers.clear();

    if (fontLoaded && titleText) {
        titleText->setString("Configurare Random");
        subtitleText->setString("Alege dimensiunea grilei");
    }

    for (int size : gridSizeOptions) {
        sf::RectangleShape button({150.f, 60.f});
        button.setFillColor(sf::Color(80, 80, 80, 200));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(150, 150, 150));
        buttons.push_back(button);

        sf::RectangleShape hover({150.f, 60.f});
        hover.setFillColor(sf::Color(255, 120, 120, 100));
        buttonHovers.push_back(hover);

        if (fontLoaded) {
            sf::Text text(font, std::to_string(size) + "x" + std::to_string(size));
            text.setCharacterSize(22);
            text.setFillColor(sf::Color::White);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = window.mapPixelToCoords({mouseMoved->position.x, mouseMoved->position.y});
        hoveredButton = -1;

        for (size_t i = 0; i < buttons.size(); i++) {
            if (buttons[i].getGlobalBounds().contains(mousePos)) {
                hoveredButton = static_cast<int>(i);
                break;
            }
        }
    }

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({mouseButton->position.x, mouseButton->position.y});

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

void GameMenu::handleModeSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].getGlobalBounds().contains(mousePos)) {
            selectedGameMode = (i == 0) ? GameMode::Score : GameMode::Mistakes;
            menuState = MenuState::SourceSelection;
            setupSourceSelectionScreen();
            break;
        }
    }
}

void GameMenu::handleSourceSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].getGlobalBounds().contains(mousePos)) {
            selectedSourceMode = (i == 0) ? SourceMode::File : SourceMode::Random;

            if (selectedSourceMode == SourceMode::File) {
                menuState = MenuState::FileSelection;
                setupFileSelectionScreen();
            } else {
                menuState = MenuState::RandomConfig;
                setupRandomConfigScreen();
            }
            break;
        }
    }
}

void GameMenu::handleFileSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].getGlobalBounds().contains(mousePos)) {
            selectedFile = availableFiles[i];
            menuState = MenuState::Starting;
            break;
        }
    }
}

void GameMenu::handleRandomConfigClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].getGlobalBounds().contains(mousePos)) {
            gridSize = gridSizeOptions[i];
            menuState = MenuState::Starting;
            break;
        }
    }
}

void GameMenu::draw(sf::RenderWindow& window) {
    switch (menuState) {
        case MenuState::ModeSelection:
            drawModeSelection(window);
            break;
        case MenuState::SourceSelection:
            drawSourceSelection(window);
            break;
        case MenuState::FileSelection:
            drawFileSelection(window);
            break;
        case MenuState::RandomConfig:
            drawRandomConfig(window);
            break;
        default:
            break;
    }
}

void GameMenu::drawModeSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    if (fontLoaded && titleText) {
        // Center title
        auto titleBounds = titleText->getLocalBounds();
        titleText->setPosition({
            window.getSize().x / 2.f - titleBounds.size.x / 2.f - titleBounds.position.x,
            80.f
        });
        window.draw(*titleText);

        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            180.f
        });
        window.draw(*subtitleText);
    }

    // Draw buttons
    float startY = 280.f;
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setPosition({
            window.getSize().x / 2.f - buttons[i].getSize().x / 2.f,
            startY + i * 80.f
        });

        if (hoveredButton == static_cast<int>(i)) {
            buttonHovers[i].setPosition(buttons[i].getPosition());
            window.draw(buttonHovers[i]);
        }

        window.draw(buttons[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                buttons[i].getPosition().x + buttons[i].getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                buttons[i].getPosition().y + buttons[i].getSize().y / 2.f - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawSourceSelection(sf::RenderWindow& window) {
    drawModeSelection(window); // Same layout
}

void GameMenu::drawFileSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    if (fontLoaded && titleText) {
        auto titleBounds = titleText->getLocalBounds();
        titleText->setPosition({
            window.getSize().x / 2.f - titleBounds.size.x / 2.f - titleBounds.position.x,
            60.f
        });
        window.draw(*titleText);

        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            140.f
        });
        window.draw(*subtitleText);
    }

    float startY = 200.f;
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setPosition({
            window.getSize().x / 2.f - buttons[i].getSize().x / 2.f,
            startY + i * 65.f
        });

        if (hoveredButton == static_cast<int>(i)) {
            buttonHovers[i].setPosition(buttons[i].getPosition());
            window.draw(buttonHovers[i]);
        }

        window.draw(buttons[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                buttons[i].getPosition().x + buttons[i].getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                buttons[i].getPosition().y + buttons[i].getSize().y / 2.f - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawRandomConfig(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    if (fontLoaded && titleText) {
        auto titleBounds = titleText->getLocalBounds();
        titleText->setPosition({
            window.getSize().x / 2.f - titleBounds.size.x / 2.f - titleBounds.position.x,
            80.f
        });
        window.draw(*titleText);

        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            160.f
        });
        window.draw(*subtitleText);
    }

    // Draw size buttons in a row
    float totalWidth = buttons.size() * 170.f - 20.f;
    float startX = window.getSize().x / 2.f - totalWidth / 2.f;
    float y = 280.f;

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].setPosition({startX + i * 170.f, y});

        if (hoveredButton == static_cast<int>(i)) {
            buttonHovers[i].setPosition(buttons[i].getPosition());
            window.draw(buttonHovers[i]);
        }

        window.draw(buttons[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                buttons[i].getPosition().x + buttons[i].getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                buttons[i].getPosition().y + buttons[i].getSize().y / 2.f - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}