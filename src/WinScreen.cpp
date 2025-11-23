#include "WinScreen.h"
#include "ShadowedText.h"
#include <fstream>
#include <iostream>

WinScreen::WinScreen() : scrollOffset(0.0f), fadeAlpha(255.0f) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    std::cerr << "Failed to load font for WinScreen\n";
  }

  if (!backgroundTexture.loadFromFile("assets/End_Poem_background.png")) {
    std::cerr << "Failed to load End_Poem_background.png\n";
  }
  backgroundSprite.emplace(backgroundTexture);

  if (!logoTexture.loadFromFile("assets/pictocraft.png")) {
    std::cerr << "Failed to load pictocraft.png\n";
  }
  logoSprite.emplace(logoTexture);

  loadPoemText();
}

WinScreen::~WinScreen() = default;

void WinScreen::loadPoemText() {
  std::ifstream file("assets/poem.txt");
  if (!file.is_open()) {
    std::cerr << "Failed to open poem.txt\n";
    return;
  }

  std::string line;
  std::string currentParagraph;
  bool useCyan = true;

  while (std::getline(file, line)) {
    if (line.empty()) {
      if (!currentParagraph.empty()) {
        paragraphs.push_back(currentParagraph);
        paragraphColors.push_back(useCyan ? sf::Color::Cyan : sf::Color::Green);
        useCyan = !useCyan;
        currentParagraph.clear();
      }
    } else {
      if (!currentParagraph.empty()) {
        currentParagraph += "\n";
      }
      currentParagraph += line;
    }
  }

  if (!currentParagraph.empty()) {
    paragraphs.push_back(currentParagraph);
    paragraphColors.push_back(useCyan ? sf::Color::Cyan : sf::Color::Green);
  }

  file.close();
}

void WinScreen::reset() {
  scrollOffset = 0.0f;
  fadeAlpha = 255.0f;
  clock.restart();
}

sf::Vector2f WinScreen::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}

void WinScreen::update(float deltaTime) {
  scrollOffset += 30.0f * deltaTime;
  if (contentHeight > 0 && scrollOffset > contentHeight) {
    scrollOffset = contentHeight;
  }
}

void WinScreen::draw(sf::RenderWindow &window) {
  auto winSize = window.getSize();
  auto [scale, scaleY] = calculateScale(window);

  auto alpha = static_cast<std::uint8_t>(std::min(fadeAlpha, 255.0f));
  backgroundSprite->setColor(sf::Color(255, 255, 255, alpha));

  float bgScaleX = static_cast<float>(winSize.x) /
                   static_cast<float>(backgroundTexture.getSize().x);
  float bgScaleY = static_cast<float>(winSize.y) /
                   static_cast<float>(backgroundTexture.getSize().y);
  backgroundSprite->setScale({bgScaleX, bgScaleY});

  window.draw(*backgroundSprite);

  float currentY = static_cast<float>(winSize.y) - scrollOffset +
                   static_cast<float>(winSize.y) * 0.2f;

  float logoScale = scale * 0.5f;
  logoSprite->setScale({logoScale, logoScale});

  auto logoBounds = logoSprite->getLocalBounds();
  float logoX =
      (static_cast<float>(winSize.x) - logoBounds.size.x * logoScale) / 2.0f;
  logoSprite->setPosition({logoX, currentY});
  logoSprite->setColor(sf::Color(255, 255, 255, alpha));

  if (currentY > -logoBounds.size.y * logoScale &&
      currentY < static_cast<float>(winSize.y)) {
    window.draw(*logoSprite);
  }

  currentY += logoBounds.size.y * logoScale + 80.0f * scale;

  unsigned int fontSize = static_cast<unsigned int>(24.0f * scale);

  for (size_t i = 0; i < paragraphs.size(); ++i) {
    sf::Text text(font);
    text.setString(paragraphs[i]);
    text.setCharacterSize(fontSize);
    text.setFillColor(paragraphColors[i]);

    auto bounds = text.getLocalBounds();
    float textX = (static_cast<float>(winSize.x) - bounds.size.x) / 2.0f -
                  bounds.position.x;
    text.setPosition({textX, currentY});

    if (currentY > -bounds.size.y && currentY < static_cast<float>(winSize.y)) {
      ShadowedText::draw(window, text, scale);
    }

    currentY += bounds.size.y + 40.0f * scale;
  }

  contentHeight =
      currentY + scrollOffset - static_cast<float>(winSize.y) * 0.5f;
  if (contentHeight < 0)
    contentHeight = 0;
}

bool WinScreen::isFinished() const {
  return contentHeight > 0 && scrollOffset >= contentHeight;
}
