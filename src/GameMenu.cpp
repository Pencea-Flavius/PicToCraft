#include "GameMenu.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

GameMenu::GameMenu()
    : fontLoaded(false),
      subtitleFontLoaded(false),
      titleLoaded(false),
      buttonLoaded(false),
      backgroundLoaded(false),
      menuState(MenuState::ModeSelection),
      selectedGameMode(GameMode::Mistakes),
      selectedSourceMode(SourceMode::File),
      selectedFile(),
      gridSize(10),
      hoveredButton(-1),
      selectedFileIndex(0),
      selectedDifficultyIndex(1)
{
    loadAssets();

    difficultyOptions = {
        {"Peaceful", 5},
        {"Normal", 8},
        {"Hard", 12},
        {"Hardcore", 16}
    };

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
        availableFiles.emplace_back("nivele/default.txt");
    }

    setupModeSelectionScreen();
}

GameMenu::~GameMenu() = default;

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

    backgroundLoaded = backgroundTexture.loadFromFile("assets/background.png");
    if (backgroundLoaded) {
        backgroundSprite = sf::Sprite(backgroundTexture);
        backgroundTexture.setRepeated(true);
    }
}

void GameMenu::createButtons(const std::vector<std::string>& labels, unsigned int fontSize) {
    buttonSprites.clear();
    buttonTexts.clear();

    for (const auto& label : labels) {
        if (buttonLoaded) {
            sf::Sprite buttonSprite(buttonTexture);
            auto bounds = buttonSprite.getLocalBounds();
            buttonSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
            buttonSprites.push_back(buttonSprite);
        }

        if (fontLoaded) {
            sf::Text text(font, label);
            text.setCharacterSize(fontSize);
            text.setFillColor(sf::Color::White);
            buttonTexts.push_back(text);
        }
    }
}

void GameMenu::createSubtitle(const std::string& text) {
    if (subtitleFontLoaded) {
        subtitleText = sf::Text(subtitleFont, text);
        subtitleText->setCharacterSize(32);
        subtitleText->setFillColor(sf::Color(220, 220, 220)); // Gri deschis
        subtitleText->setOutlineThickness(2.0f);
        subtitleText->setOutlineColor(sf::Color::Black);
    }
}

void GameMenu::setupModeSelectionScreen() {
    subtitleText.reset();
    createButtons({"Mod Scor", "Mod Greseli", "Quit"}, 20);
}

void GameMenu::setupSourceSelectionScreen() {
    createSubtitle(selectedGameMode == GameMode::Score ?
        "Mod: Scor" : "Mod: Greseli (3 maxim)");
    createButtons({"Joc din Fisier", "Joc Random", "Back"}, 20);
}

void GameMenu::setupFileSelectionScreen() {
    createSubtitle("Alege nivelul");

    std::vector<std::string> fileNames;
    for (const auto& filePath : availableFiles) {
        std::filesystem::path p(filePath);
        fileNames.push_back(p.stem().string());
    }
    fileNames.emplace_back("Back");

    createButtons(fileNames, 18);
}

void GameMenu::setupRandomConfigScreen() {
    createSubtitle("Alege dificultatea");

    std::vector<std::string> difficultyNames;
    for (const auto& difficulty : difficultyOptions) {
        difficultyNames.push_back(difficulty.name);
    }
    difficultyNames.emplace_back("Back");

    createButtons(difficultyNames, 20);
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
            if (i == 2) {
                menuState = MenuState::Quitting;
            } else {
                selectedGameMode = (i == 0) ? GameMode::Score : GameMode::Mistakes;
                menuState = MenuState::SourceSelection;
                setupSourceSelectionScreen();
            }
            break;
        }
    }
}

void GameMenu::handleSourceSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
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

void GameMenu::handleFileSelectionClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
            if (i == buttonSprites.size() - 1) {
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

void GameMenu::handleRandomConfigClick(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < buttonSprites.size(); i++) {
        if (buttonSprites[i].getGlobalBounds().contains(mousePos)) {
            if (i == buttonSprites.size() - 1) {
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

void GameMenu::drawBackground(sf::RenderWindow& window) {
    if (backgroundLoaded && backgroundSprite) {
        backgroundSprite->setScale({
            static_cast<float>(window.getSize().x) / static_cast<float>(backgroundTexture.getSize().x),
            static_cast<float>(window.getSize().y) / static_cast<float>(backgroundTexture.getSize().y)
        });
        window.draw(*backgroundSprite);
    }
}

sf::Vector2f GameMenu::calculateScale(const sf::RenderWindow& window) const {
    float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
    float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
    float scale = std::min(scaleX, scaleY);
    return {scale, scaleY};
}

void GameMenu::drawSubtitle(sf::RenderWindow& window, float yPosition) {
    if (subtitleFontLoaded && subtitleText) {
        auto [scale, scaleY] = calculateScale(window);
        subtitleText->setCharacterSize(static_cast<unsigned int>(36.0f * scale));
        auto subtitleBounds = subtitleText->getLocalBounds();
        subtitleText->setPosition({
            static_cast<float>(window.getSize().x) / 2.0f - subtitleBounds.size.x / 2.0f - subtitleBounds.position.x,
            yPosition * scaleY
        });
        window.draw(*subtitleText);
    }
}

void GameMenu::drawButtons(sf::RenderWindow& window, float startY, float spacing,
                           float buttonScaleX, float buttonScaleY, float textSize) {
    auto [scale, scaleY] = calculateScale(window);

    for (size_t i = 0; i < buttonSprites.size(); i++) {
        buttonSprites[i].setScale({scale * buttonScaleX, scale * buttonScaleY});
        buttonSprites[i].setPosition({
            static_cast<float>(window.getSize().x) / 2.0f,
            startY + static_cast<float>(i) * spacing
        });

        if (hoveredButton == static_cast<int>(i)) {
            buttonSprites[i].setColor(sf::Color(160, 190, 240));
        } else {
            buttonSprites[i].setColor(sf::Color::White);
        }

        window.draw(buttonSprites[i]);

        if (fontLoaded && i < buttonTexts.size()) {
            buttonTexts[i].setCharacterSize(static_cast<unsigned int>(textSize * scale));

            auto textBounds = buttonTexts[i].getLocalBounds();
            float textX = static_cast<float>(window.getSize().x) / 2.0f - textBounds.size.x / 2.0f - textBounds.position.x;
            float textY = startY + static_cast<float>(i) * spacing - textBounds.size.y / 2.0f - textBounds.position.y;
            sf::Text shadowText = buttonTexts[i];
            shadowText.setFillColor(sf::Color(0, 0, 0, 170));
            shadowText.setPosition({textX + 2.0f * scale, textY + 2.0f * scale});
            window.draw(shadowText);

            //Text Buton
            if (hoveredButton == static_cast<int>(i)) {
                buttonTexts[i].setFillColor(sf::Color(255, 255, 160)); // Galben deschis
            } else {
                buttonTexts[i].setFillColor(sf::Color(221, 221, 221)); // Gri deschis (#DDD)
            }

            buttonTexts[i].setPosition({textX, textY});
            window.draw(buttonTexts[i]);
        }
    }
}

void GameMenu::drawModeSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));
    auto [scale, scaleY] = calculateScale(window);

    drawBackground(window);

    if (titleLoaded && titleSprite) {
        float logoScale = scale * 0.5f;
        titleSprite->setScale({logoScale, logoScale});
        titleSprite->setPosition({static_cast<float>(window.getSize().x) / 2.0f, 100.0f * scaleY});
        window.draw(*titleSprite);
    }

    drawButtons(window, 280.0f * scaleY, 80.0f * scaleY, 1.2f, 1.2f, 20.0f);
}

void GameMenu::drawSourceSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));
    drawBackground(window);
    drawSubtitle(window, 80.0f);
    auto [scale, scaleY] = calculateScale(window);
    drawButtons(window, 200.0f * scaleY, 80.0f * scaleY, 1.2f, 1.2f, 20.0f);
}

void GameMenu::drawFileSelection(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));
    drawBackground(window);
    drawSubtitle(window, 60.0f);
    auto [scale, scaleY] = calculateScale(window);
    drawButtons(window, 160.0f * scaleY, 70.0f * scaleY, 1.4f, 1.0f, 18.0f);
}

void GameMenu::drawRandomConfig(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 40));
    drawBackground(window);
    drawSubtitle(window, 80.0f);
    auto [scale, scaleY] = calculateScale(window);
    drawButtons(window, 200.0f * scaleY, 85.0f * scaleY, 1.2f, 1.0f, 20.0f);
}