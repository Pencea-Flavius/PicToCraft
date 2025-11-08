//
// Created by zzfla on 11/8/2025.
//

#include "MenuResolution.h"
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

static bool loadMenuFont(sf::Font &font) {
    const char *path = "assets/Monocraft.ttf";
    return font.openFromFile(path);
}

static std::string resolutionToString(const sf::Vector2u &v) {
    return std::to_string(v.x) + " x " + std::to_string(v.y);
}

MenuResolution &MenuResolution::getInstance() {
    static MenuResolution instance;
    return instance;
}

sf::VideoMode MenuResolution::selectResolution() {
    std::vector<sf::Vector2u> options = {
        {1280, 720}, {1600, 900}, {1920, 1080}, {2560, 1440}, {3840, 2160}
    };

    auto desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2u deskSize{desktop.size.x, desktop.size.y};
    auto exists = std::find(options.begin(), options.end(), deskSize) != options.end();
    if (!exists) options.push_back(deskSize);

    std::size_t current = 0;

    sf::RenderWindow window(sf::VideoMode({1100, 450}), "Selecteaza rezolutia");
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    bool haveFont = loadMenuFont(font);

    std::unique_ptr<sf::Text> instructions;
    std::unique_ptr<sf::Text> rowFullscreen;
    std::unique_ptr<sf::Text> rowWindowed;
    if (haveFont) {
        instructions = std::make_unique<sf::Text>(
            font,
            "Sus/Jos: rand | Stanga/Dreapta: rezolutie | Enter: OK | Esc: iesire"
        );
        rowFullscreen = std::make_unique<sf::Text>(font, "");
        rowWindowed = std::make_unique<sf::Text>(font, "");

        instructions->setCharacterSize(22);
        instructions->setFillColor(sf::Color(200, 200, 200));
        instructions->setPosition({20.f, 18.f});

        rowFullscreen->setCharacterSize(32);
        rowWindowed->setCharacterSize(32);
    }

    fullscreenChosen_ = false;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                    return sf::VideoMode({deskSize.x, deskSize.y});
                }
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    if (fullscreenChosen_) {
                        auto dm = sf::VideoMode::getDesktopMode();
                        window.close();
                        return sf::VideoMode({dm.size.x, dm.size.y});
                    } else {
                        auto sel = options[current];
                        window.close();
                        return sf::VideoMode({sel.x, sel.y});
                    }
                }
                if (key->code == sf::Keyboard::Key::Right) {
                    if (!fullscreenChosen_) current = (current + 1) % options.size();
                }
                if (key->code == sf::Keyboard::Key::Left) {
                    if (!fullscreenChosen_) current = (current + options.size() - 1) % options.size();
                }
                if (key->code == sf::Keyboard::Key::Up) {
                    fullscreenChosen_ = true;
                }
                if (key->code == sf::Keyboard::Key::Down) {
                    fullscreenChosen_ = false;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        auto sel = options[current];

        if (haveFont) {
            // Build strings for both rows
            std::string fsStr = std::string("Fullscreen (desktop: ") + resolutionToString(deskSize) + ")";
            std::string winStr = std::string("Fereastra | Rezolutie: ") + resolutionToString(sel);

            rowFullscreen->setString(fsStr);
            rowWindowed->setString(winStr);

            // Colors based on selection
            if (fullscreenChosen_) {
                rowFullscreen->setFillColor(sf::Color(255, 255, 255));
                rowWindowed->setFillColor(sf::Color(180, 180, 180));
            } else {
                rowFullscreen->setFillColor(sf::Color(180, 180, 180));
                rowWindowed->setFillColor(sf::Color(255, 255, 255));
            }

            // Center and position rows
            auto fsBounds = rowFullscreen->getLocalBounds();
            auto winBounds = rowWindowed->getLocalBounds();
            float fsX = (window.getSize().x - fsBounds.size.x) * 0.5f - fsBounds.position.x;
            float winX = (window.getSize().x - winBounds.size.x) * 0.5f - winBounds.position.x;
            float fsY = 110.f - fsBounds.position.y;
            float winY = 170.f - winBounds.position.y;
            rowFullscreen->setPosition({fsX, fsY});
            rowWindowed->setPosition({winX, winY});

            // Draw
            window.draw(*instructions);
            window.draw(*rowFullscreen);
            window.draw(*rowWindowed);

            // Margin arrows aligned with the selected row
            float arrowY = fullscreenChosen_ ? (fsY + fsBounds.size.y * 0.5f) : (winY + winBounds.size.y * 0.5f);
            sf::Color arrowColor(230, 230, 230);

            sf::CircleShape leftArrow(10.f, 3);
            leftArrow.setRotation(sf::degrees(90));
            leftArrow.setFillColor(arrowColor);
            leftArrow.setPosition({40.f, arrowY});
            window.draw(leftArrow);

            sf::CircleShape rightArrow(10.f, 3);
            rightArrow.setRotation(sf::degrees(270));
            rightArrow.setFillColor(arrowColor);
            rightArrow.setPosition({window.getSize().x - 40.f, arrowY});
            window.draw(rightArrow);
        }

        window.display();
    }

    auto sel = options[current];
    return sf::VideoMode({sel.x, sel.y});
}