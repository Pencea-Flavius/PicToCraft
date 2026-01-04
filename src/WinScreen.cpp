#include "WinScreen.h"
#include "Exceptions.h"
#include "ShadowedText.h"
#include <fstream>
#include <iostream>

WinScreen::WinScreen() : scrollOffset(0.0f), fadeAlpha(255.0f), clock(), speedMultiplier(1.0f) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    throw AssetLoadException("assets/Monocraft.ttf", "Font");
  }

  if (!backgroundTexture.loadFromFile("assets/End_Poem_background.png")) {
    throw AssetLoadException("assets/End_Poem_background.png", "Texture");
  }
  backgroundSprite.emplace(backgroundTexture);

  if (!logoTexture.loadFromFile("assets/pictocraft.png")) {
    throw AssetLoadException("assets/pictocraft.png", "Texture");
  }
  logoSprite.emplace(logoTexture);

  loadPoemText();
}

WinScreen::~WinScreen() = default;

void WinScreen::loadPoemText() {
  std::ifstream file("assets/poem.txt");
  if (!file.is_open()) {
    throw AssetLoadException("assets/poem.txt", "File");
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
  speedMultiplier = 1.0f;
}

void WinScreen::setScore(int score, const Leaderboard& leaderboard) {
  finalScore = score;
  topScores = leaderboard.getEntries();
  loadPoemText();
}

sf::Vector2f WinScreen::calculateScale(const sf::RenderWindow &window) const {
  float scaleX = static_cast<float>(window.getSize().x) / baseWidth;
  float scaleY = static_cast<float>(window.getSize().y) / baseHeight;
  float scale = std::min(scaleX, scaleY);
  return {scale, scaleY};
}

void WinScreen::update(float deltaTime) {
  scrollOffset += 30.0f * deltaTime * speedMultiplier;
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

  // Draw Score
  sf::Text scoreText(font, "Final Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(static_cast<unsigned int>(40.0f * scale));
  scoreText.setFillColor(sf::Color::Yellow);
  auto scoreBounds = scoreText.getLocalBounds();
  float scoreX = (static_cast<float>(winSize.x) - scoreBounds.size.x) / 2.0f -
                 scoreBounds.position.x;
  scoreText.setPosition({scoreX, currentY});

  if (currentY > -scoreBounds.size.y && currentY < static_cast<float>(winSize.y)) {
    ShadowedText::draw(window, scoreText, scale);
  }

  currentY += scoreBounds.size.y + 20.0f * scale;

  // Draw Leaderboard
  if (!topScores.empty()) {
      sf::Text lbHeader(font, "=== LEADERBOARD ===");
      lbHeader.setCharacterSize(static_cast<unsigned int>(30.0f * scale));
      lbHeader.setFillColor(sf::Color::Cyan);
      auto lbHeaderBounds = lbHeader.getLocalBounds();
      lbHeader.setPosition({(static_cast<float>(winSize.x) - lbHeaderBounds.size.x) / 2.0f, currentY});
      ShadowedText::draw(window, lbHeader, scale);
      
      currentY += lbHeaderBounds.size.y + 20.0f * scale;
      
      for (const auto& entry : topScores) {
          std::string line = entry.name + " ........ " + std::to_string(entry.score);
          sf::Text entryText(font, line);
          entryText.setCharacterSize(static_cast<unsigned int>(24.0f * scale));
          entryText.setFillColor(sf::Color::White);
          auto entryBounds = entryText.getLocalBounds();
          entryText.setPosition({(static_cast<float>(winSize.x) - entryBounds.size.x) / 2.0f, currentY});
          ShadowedText::draw(window, entryText, scale);
          currentY += entryBounds.size.y + 10.0f * scale;
      }
      currentY += 40.0f * scale;
  } else {
       currentY += 40.0f * scale;
  }

  auto fontSize = static_cast<unsigned int>(24.0f * scale);

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

void WinScreen::setSpeedMultiplier(float multiplier) {
  speedMultiplier = multiplier;
}
