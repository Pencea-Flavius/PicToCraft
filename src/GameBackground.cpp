#include "GameBackground.h"
#include <iostream>
#include <random>

GameBackground::GameBackground() : currentType(BackgroundType::Plains) {
  loadTextures();
}

void GameBackground::loadTextures() {
  if (!plainsTexture.loadFromFile("assets/backgrounds/plains.png")) {
    std::cerr << "Failed to load plains background" << std::endl;
  }
  if (!desertTexture.loadFromFile("assets/backgrounds/desert.png")) {
    std::cerr << "Failed to load desert background" << std::endl;
  }
  if (!caveTexture.loadFromFile("assets/backgrounds/cave.png")) {
    std::cerr << "Failed to load cave background" << std::endl;
  }
  if (!mineshaftTexture.loadFromFile("assets/backgrounds/mineshaft.png")) {
    std::cerr << "Failed to load mineshaft background" << std::endl;
  }
}

void GameBackground::selectBackground(const GameConfig &config) {
  BackgroundType selectedType;

  if (config.timeMode && config.spidersMode) {
    selectedType = BackgroundType::Mineshaft;
  } else if (config.torchMode || config.spidersMode) {
    selectedType = BackgroundType::Cave;
  } else {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    selectedType =
        (dis(gen) == 0) ? BackgroundType::Plains : BackgroundType::Desert;
  }

  currentType = selectedType;

  switch (currentType) {
  case BackgroundType::Plains:
    currentBackground = sf::Sprite(plainsTexture);
    break;
  case BackgroundType::Desert:
    currentBackground = sf::Sprite(desertTexture);
    break;
  case BackgroundType::Cave:
    currentBackground = sf::Sprite(caveTexture);
    break;
  case BackgroundType::Mineshaft:
    currentBackground = sf::Sprite(mineshaftTexture);
    break;
  }
}

void GameBackground::draw(sf::RenderWindow &window) const {
  if (!currentBackground)
    return;

  auto winSize = window.getSize();
  auto texSize = currentBackground->getTexture().getSize();

  float scaleX = static_cast<float>(winSize.x) / static_cast<float>(texSize.x);
  float scaleY = static_cast<float>(winSize.y) / static_cast<float>(texSize.y);

  currentBackground->setScale({scaleX, scaleY});
  currentBackground->setPosition({0.f, 0.f});

  window.draw(*currentBackground);
}
