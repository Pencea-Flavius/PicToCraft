#include "GameMenu.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

GameMenu::GameMenu()
    : fontLoaded(false),
      titleLoaded(false),
      buttonLoaded(false),
      backgroundLoaded(false),
      menuState(MenuState::ModeSelection),
      selectedGameMode(GameMode::Mistakes),
      selectedSourceMode(SourceMode::File),
      selectedFile(""),
      gridSize(10),
      hoveredButton(-1),
      selectedFileIndex(0),
      selectedDifficultyIndex(1)
{
    loadAssets();

    // Difficulty options cu nume și dimensiuni
    difficultyOptions = {
        {"Peaceful", 5},
        {"Normal", 8},
        {"Hard", 12},
        {"Hardcore", 16}
    };

    // Scan for available files DOAR din folderul "nivele"
    try {
        std::filesystem::path levelDir("nivele");
        if (std::filesystem::exists(levelDir) && std::filesystem::is_directory(levelDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(levelDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.ends_with(".txt")) {
                        availableFiles.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Eroare la citirea folderului nivele: " << e.what() << "\n";
    }

    if (availableFiles.empty()) {
        std::cerr << "Nici un fisier gasit in folderul nivele!\n";
        availableFiles.push_back("nivele/default.txt");
    }

    setupModeSelectionScreen();
}

void GameMenu::loadAssets() {
    fontLoaded = font.openFromFile("assets/Monocraft.ttf");

    titleLoaded = titleTexture.loadFromFile("assets/pictocraft.png");
    if (titleLoaded) {
        titleSprite = sf::Sprite(titleTexture);
        auto bounds = titleSprite->getLocalBounds();
        titleSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    }

    buttonLoaded = buttonTexture.loadFromFile("assets/button.png");

    backgroundLoaded = backgroundTexture.loadFromFile("assets/background.png");
    if (backgroundLoaded) {
        backgroundSprite = sf::Sprite(backgroundTexture);
        backgroundTexture.setRepeated(true);
    }
}

void GameMenu::setupModeSelectionScreen() {
    buttonSprites.clear();
    buttonTexts.clear();

    // NU mai avem subtitle pentru primul ecran
    subtitleText.reset();

    // Create buttons
    std::vector<std::string> labels = {"Mod Scor", "Mod Greseli"};

    for (size_t i = 0; i < labels.size(); i++) {
        if (buttonLoaded) {
            sf::Sprite buttonSprite(buttonTexture);
            auto bounds = buttonSprite.getLocalBounds();
            buttonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            buttonSprites.push_back(buttonSprite);
        }

        if (fontLoaded) {
            sf::Text text(font, labels[i]);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setOutlineThickness(1);
            text.setOutlineColor(sf::Color::Black);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupSourceSelectionScreen() {
    buttonSprites.clear();
    buttonTexts.clear();

    // Subtitle pentru celelalte ecrane
    if (fontLoaded) {
        subtitleText = sf::Text(font, selectedGameMode == GameMode::Score ?
            "Mod: Scor" : "Mod: Greseli (3 maxim)");
        subtitleText->setCharacterSize(24);
        subtitleText->setFillColor(sf::Color(200, 200, 200));
        subtitleText->setOutlineThickness(1);
        subtitleText->setOutlineColor(sf::Color::Black);
    }

    std::vector<std::string> labels = {"Joc din Fisier", "Joc Random"};

    for (size_t i = 0; i < labels.size(); i++) {
        if (buttonLoaded) {
            sf::Sprite buttonSprite(buttonTexture);
            auto bounds = buttonSprite.getLocalBounds();
            buttonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            buttonSprites.push_back(buttonSprite);
        }

        if (fontLoaded) {
            sf::Text text(font, labels[i]);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setOutlineThickness(1);
            text.setOutlineColor(sf::Color::Black);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupFileSelectionScreen() {
    buttonSprites.clear();
    buttonTexts.clear();

    if (fontLoaded) {
        subtitleText = sf::Text(font, "Alege nivelul");
        subtitleText->setCharacterSize(24);
        subtitleText->setFillColor(sf::Color(200, 200, 200));
        subtitleText->setOutlineThickness(1);
        subtitleText->setOutlineColor(sf::Color::Black);
    }

    for (const auto& filePath : availableFiles) {
        if (buttonLoaded) {
            sf::Sprite buttonSprite(buttonTexture);
            auto bounds = buttonSprite.getLocalBounds();
            buttonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            buttonSprites.push_back(buttonSprite);
        }

        if (fontLoaded) {
            // Extrage doar numele fișierului fără path
            std::filesystem::path p(filePath);
            std::string displayName = p.filename().string();
            // Elimină extensia .txt
            if (displayName.ends_with(".txt")) {
                displayName = displayName.substr(0, displayName.length() - 4);
            }

            sf::Text text(font, displayName);
            text.setCharacterSize(18);
            text.setFillColor(sf::Color::White);
            text.setOutlineThickness(1);
            text.setOutlineColor(sf::Color::Black);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::setupRandomConfigScreen() {
    buttonSprites.clear();
    buttonTexts.clear();

    if (fontLoaded) {
        subtitleText = sf::Text(font, "Alege dificultatea");
        subtitleText->setCharacterSize(24);
        subtitleText->setFillColor(sf::Color(200, 200, 200));
        subtitleText->setOutlineThickness(1);
        subtitleText->setOutlineColor(sf::Color::Black);
    }

    for (const auto& difficulty : difficultyOptions) {
        if (buttonLoaded) {
            sf::Sprite buttonSprite(buttonTexture);
            auto bounds = buttonSprite.getLocalBounds();
            buttonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            buttonSprites.push_back(buttonSprite);
        }

        if (fontLoaded) {
            sf::Text text(font, difficulty.name);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setOutlineThickness(1);
            text.setOutlineColor(sf::Color::Black);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = window.mapPixelToCoords({mouseMoved->position.x, mouseMoved->position.y});
        hoveredButton = -1;

        for (size_t i = 0; i < buttonSprites.size(); i++) {
            if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
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
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
            selectedGameMode = (i == 0) ? GameMode::Score : GameMode::Mistakes;
            menuState = MenuState::SourceSelection;
            setupSourceSelectionScreen();
            break;
        }
    }
}

void GameMenu::handleSourceSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
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
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
            selectedFile = availableFiles[i];
            menuState = MenuState::Starting;
            break;
        }
    }
}

void GameMenu::handleRandomConfigClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
            gridSize = difficultyOptions[i].gridSize;
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

    float scaleX = window.getSize().x / baseWidth;
    float scaleY = window.getSize().y / baseHeight;
    float scale = std::min(scaleX, scaleY);

    // Draw background
    if (backgroundLoaded && backgroundSprite) {
        backgroundSprite->setScale({
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        });
        window.draw(*backgroundSprite);
    }

    // Draw logo - scalat corect pentru dimensiunile 1003x162
    if (titleLoaded && titleSprite) {
        // Calculează scalarea pentru logo
        float logoScale = scale * 0.5f; // Ajustează acest factor pentru dimensiunea dorită
        titleSprite->setScale({logoScale, logoScale});
        titleSprite->setPosition({window.getSize().x / 2.f, 100.f * scaleY});
        window.draw(*titleSprite);
    }

    // NU desenăm subtitle pe primul ecran

    // Draw buttons
    float startY = 280.f * scaleY;
    float spacing = 80.f * scaleY;

    for (size_t i = 0; i < buttonSprites.size(); i++) {
        buttonSprites[i].setScale({scale * 1.2f, scale * 1.2f});
        buttonSprites[i].setPosition({window.getSize().x / 2.f, startY + i * spacing});

        // Butonul devine albastru când e hover
        if (hoveredButton == static_cast<int>(i)) {
            buttonSprites[i].setColor(sf::Color(100, 150, 255)); // Albastru deschis
        } else {
            buttonSprites[i].setColor(sf::Color::White);
        }

        window.draw(buttonSprites[i]);

        // Textul devine galben când e hover
        if (fontLoaded && i < buttonTexts.size()) {
            buttonTexts[i].setCharacterSize(static_cast<unsigned int>(20 * scale));

            if (hoveredButton == static_cast<int>(i)) {
                buttonTexts[i].setFillColor(sf::Color(255, 255, 100)); // Galben
            } else {
                buttonTexts[i].setFillColor(sf::Color::White);
            }

            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                window.getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                startY + i * spacing - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawSourceSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    float scaleX = window.getSize().x / baseWidth;
    float scaleY = window.getSize().y / baseHeight;
    float scale = std::min(scaleX, scaleY);

    if (backgroundLoaded && backgroundSprite) {
        backgroundSprite->setScale({
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        });
        window.draw(*backgroundSprite);
    }

    // Draw subtitle (NU logo)
    if (fontLoaded && subtitleText) {
        subtitleText->setCharacterSize(static_cast<unsigned int>(28 * scale));
        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            80.f * scaleY
        });
        window.draw(*subtitleText);
    }

    float startY = 200.f * scaleY;
    float spacing = 80.f * scaleY;

    for (size_t i = 0; i < buttonSprites.size(); i++) {
        buttonSprites[i].setScale({scale * 1.2f, scale * 1.2f});
        buttonSprites[i].setPosition({window.getSize().x / 2.f, startY + i * spacing});

        if (hoveredButton == static_cast<int>(i)) {
            buttonSprites[i].setColor(sf::Color(100, 150, 255));
        } else {
            buttonSprites[i].setColor(sf::Color::White);
        }

        window.draw(buttonSprites[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            buttonTexts[i].setCharacterSize(static_cast<unsigned int>(20 * scale));

            if (hoveredButton == static_cast<int>(i)) {
                buttonTexts[i].setFillColor(sf::Color(255, 255, 100));
            } else {
                buttonTexts[i].setFillColor(sf::Color::White);
            }

            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                window.getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                startY + i * spacing - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawFileSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    float scaleX = window.getSize().x / baseWidth;
    float scaleY = window.getSize().y / baseHeight;
    float scale = std::min(scaleX, scaleY);

    if (backgroundLoaded && backgroundSprite) {
        backgroundSprite->setScale({
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        });
        window.draw(*backgroundSprite);
    }

    if (fontLoaded && subtitleText) {
        subtitleText->setCharacterSize(static_cast<unsigned int>(28 * scale));
        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            60.f * scaleY
        });
        window.draw(*subtitleText);
    }

    float startY = 160.f * scaleY;
    float spacing = 70.f * scaleY;

    for (size_t i = 0; i < buttonSprites.size(); i++) {
        buttonSprites[i].setScale({scale * 1.4f, scale});
        buttonSprites[i].setPosition({window.getSize().x / 2.f, startY + i * spacing});

        if (hoveredButton == static_cast<int>(i)) {
            buttonSprites[i].setColor(sf::Color(100, 150, 255));
        } else {
            buttonSprites[i].setColor(sf::Color::White);
        }

        window.draw(buttonSprites[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            buttonTexts[i].setCharacterSize(static_cast<unsigned int>(18 * scale));

            if (hoveredButton == static_cast<int>(i)) {
                buttonTexts[i].setFillColor(sf::Color(255, 255, 100));
            } else {
                buttonTexts[i].setFillColor(sf::Color::White);
            }

            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                window.getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                startY + i * spacing - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawRandomConfig(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));

    float scaleX = window.getSize().x / baseWidth;
    float scaleY = window.getSize().y / baseHeight;
    float scale = std::min(scaleX, scaleY);

    if (backgroundLoaded && backgroundSprite) {
        backgroundSprite->setScale({
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        });
        window.draw(*backgroundSprite);
    }

    if (fontLoaded && subtitleText) {
        subtitleText->setCharacterSize(static_cast<unsigned int>(28 * scale));
        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            window.getSize().x / 2.f - subtitleBounds.size.x / 2.f - subtitleBounds.position.x,
            80.f * scaleY
        });
        window.draw(*subtitleText);
    }

    // Butoane în coloană pentru mai mult spațiu
    float startY = 200.f * scaleY;
    float spacing = 85.f * scaleY;

    for (size_t i = 0; i < buttonSprites.size(); i++) {
        buttonSprites[i].setScale({scale * 1.2f, scale});
        buttonSprites[i].setPosition({window.getSize().x / 2.f, startY + i * spacing});

        if (hoveredButton == static_cast<int>(i)) {
            buttonSprites[i].setColor(sf::Color(100, 150, 255));
        } else {
            buttonSprites[i].setColor(sf::Color::White);
        }

        window.draw(buttonSprites[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            buttonTexts[i].setCharacterSize(static_cast<unsigned int>(20 * scale));

            if (hoveredButton == static_cast<int>(i)) {
                buttonTexts[i].setFillColor(sf::Color(255, 255, 100));
            } else {
                buttonTexts[i].setFillColor(sf::Color::White);
            }

            auto textBounds = buttonTexts[i].getLocalBounds();
            buttonTexts[i].setPosition({
                window.getSize().x / 2.f - textBounds.size.x / 2.f - textBounds.position.x,
                startY + i * spacing - textBounds.size.y / 2.f - textBounds.position.y
            });
            window.draw(buttonTexts[i]);
        }
    }
}