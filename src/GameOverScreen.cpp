#include "GameOverScreen.h"
#include "Exceptions.h"
#include "ShadowedText.h"
#include <iostream>

GameOverScreen::GameOverScreen()
    : font(), titleText(font), scoreLabel(font), scoreValue(font) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    throw AssetLoadException("assets/Monocraft.ttf", "Font");
  }

  titleText.setString("You died!");
  titleText.setCharacterSize(60);
  titleText.setFillColor(sf::Color::White);

  scoreLabel.setString("Score: ");
  scoreLabel.setCharacterSize(30);
  scoreLabel.setFillColor(sf::Color::White);

  scoreValue.setCharacterSize(30);
  scoreValue.setFillColor(sf::Color::Yellow);

  overlay.setFillColor(sf::Color(100, 0, 0, 150)); // Semi-transparent red

  createButtons();
}

GameOverScreen::~GameOverScreen() = default;

void GameOverScreen::createButtons() {
  static sf::Texture buttonTexture;
  if (buttonTexture.getSize().x == 0) {
    if (!buttonTexture.loadFromFile("assets/buttons/button.png")) {
      throw AssetLoadException("assets/buttons/button.png", "Texture");
    }
  }

  buttons.push_back(
      std::make_unique<MenuButton>("Try again", font, buttonTexture, 20));
  buttons.push_back(
      std::make_unique<MenuButton>("Main Menu", font, buttonTexture, 20));
}

void GameOverScreen::setScore(int score) {
  scoreValue.setString(std::to_string(score));
}

GameOverAction
GameOverScreen::handleEvent(const sf::Event &event,
                            const sf::RenderWindow &window) const {
  if (const auto *mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseButton->button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = window.mapPixelToCoords(
          {mouseButton->position.x, mouseButton->position.y});

      if (buttons[0]->isClicked(mousePos)) {
        return GameOverAction::Retry;
      }
      if (buttons[1]->isClicked(mousePos)) {
        return GameOverAction::MainMenu;
      }
    }
  }
  return GameOverAction::None;
}

sf::Vector2f
GameOverScreen::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}

void GameOverScreen::update(const sf::RenderWindow &window) {
  sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
  sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
  auto winSize = window.getSize();
  auto [scale, scaleY] = calculateScale(window);

  // Update overlay size
  overlay.setSize(sf::Vector2f(static_cast<float>(winSize.x),
                               static_cast<float>(winSize.y)));

  // Scale and Position Title
  titleText.setCharacterSize(static_cast<unsigned int>(60.0f * scale));
  auto titleBounds = titleText.getLocalBounds();
  titleText.setPosition(
      {(static_cast<float>(winSize.x) - titleBounds.size.x) / 2.0f -
           titleBounds.position.x,
       static_cast<float>(winSize.y) * 0.3f});

  // Scale and Position Score
  scoreLabel.setCharacterSize(static_cast<unsigned int>(30.0f * scale));
  scoreValue.setCharacterSize(static_cast<unsigned int>(30.0f * scale));

  auto labelBounds = scoreLabel.getLocalBounds();
  auto valueBounds = scoreValue.getLocalBounds();
  float totalWidth = labelBounds.size.x + valueBounds.size.x;

  float startX = (static_cast<float>(winSize.x) - totalWidth) / 2.0f;
  float scoreY = static_cast<float>(winSize.y) * 0.45f;

  scoreLabel.setPosition({startX, scoreY});
  scoreValue.setPosition({startX + labelBounds.size.x, scoreY});

  // Update Buttons
  float buttonY = static_cast<float>(winSize.y) * 0.6f;
  float spacing = 60.0f * scaleY;

  for (size_t i = 0; i < buttons.size(); ++i) {
    float x = static_cast<float>(winSize.x) / 2.0f;
    float y = buttonY + static_cast<float>(i) * spacing;
    buttons[i]->update(scale, x, y, 1.2f, 1.2f, mousePos);
  }
}

void GameOverScreen::draw(sf::RenderWindow &window) const {
  auto [scale, scaleY] = calculateScale(window);

  window.draw(overlay);

  ShadowedText::draw(window, titleText, scale);
  ShadowedText::draw(window, scoreLabel, scale);
  ShadowedText::draw(window, scoreValue, scale);

  for (const auto &btn : buttons) {
    btn->draw(window);
  }
}

void GameOverScreen::reset() { scoreValue.setString(""); }
