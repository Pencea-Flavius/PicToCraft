#include "PauseMenu.h"
#include "Exceptions.h"
#include "ShadowedText.h"
#include <cmath>

PauseMenu::PauseMenu() : font(), titleText(font) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    throw AssetLoadException("assets/Monocraft.ttf", "Font");
  }

  titleText.setString("Game Paused");
  titleText.setCharacterSize(60);
  titleText.setFillColor(sf::Color::White);

  overlay.setFillColor(sf::Color(0, 0, 0, 180)); // Darker black overlay

  createButtons();
}

void PauseMenu::createButtons() {
  static sf::Texture buttonTexture;
  if (buttonTexture.getSize().x == 0) {
    if (!buttonTexture.loadFromFile("assets/buttons/button.png")) {
      throw AssetLoadException("assets/buttons/button.png", "Texture");
    }
  }

  buttons.push_back(
      std::make_unique<MenuButton>("Resume Game", font, buttonTexture, 20));
  buttons.push_back(
      std::make_unique<MenuButton>("Exit to Menu", font, buttonTexture, 20));
}

sf::Vector2f PauseMenu::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}

PauseAction PauseMenu::handleEvent(const sf::Event &event,
                                   const sf::RenderWindow &window) const {
  if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseButton->button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = window.mapPixelToCoords(
          {mouseButton->position.x, mouseButton->position.y});

      if (buttons[0]->isClicked(mousePos)) {
        return PauseAction::Resume;
      }
      if (buttons[1]->isClicked(mousePos)) {
        return PauseAction::MainMenu;
      }
    }
  } else if (auto key = event.getIf<sf::Event::KeyPressed>()) {
      if (key->code == sf::Keyboard::Key::Escape) {
          return PauseAction::Resume;
      }
  }
  return PauseAction::None;
}

void PauseMenu::update(const sf::RenderWindow &window) {
  sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
  sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
  auto winSize = window.getSize();
  auto [scale, scaleY] = calculateScale(window);

  // Update overlay
  overlay.setSize(sf::Vector2f(static_cast<float>(winSize.x),
                               std::round(static_cast<float>(winSize.y))));
  overlay.setPosition({0.f, 0.f});

  // Title
  titleText.setCharacterSize(static_cast<unsigned int>(60.0f * scale));
  auto titleBounds = titleText.getLocalBounds();
  titleText.setPosition(
      {(static_cast<float>(winSize.x) - titleBounds.size.x) / 2.0f -
           titleBounds.position.x,
       static_cast<float>(winSize.y) * 0.3f});

  // Buttons
  float buttonY = static_cast<float>(winSize.y) * 0.5f;
  float spacing = 70.0f * scaleY;

  for (size_t i = 0; i < buttons.size(); ++i) {
    float x = static_cast<float>(winSize.x) / 2.0f;
    float y = buttonY + static_cast<float>(i) * spacing;
    buttons[i]->update(scale, x, y, 1.2f, 1.2f, mousePos);
  }
}

void PauseMenu::draw(sf::RenderWindow &window) const {
  auto [scale, scaleY] = calculateScale(window);

  window.draw(overlay);
  ShadowedText::draw(window, titleText, scale);

  for (const auto &btn : buttons) {
    btn->draw(window);
  }
}
