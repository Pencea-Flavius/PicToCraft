#include "GameBackground.h"
#include <iostream>
#include <random>

#include "GameBackground.h"
#include <iostream>
#include <random>

GameBackground::GameBackground() : currentType(BackgroundType::Desert), offset(0.0f), speed(30.0f), loaded(false) {
  loadTextures();
}

void GameBackground::loadTextures() {
  if (!desertTexture.loadFromFile("assets/backgrounds/desert.jpg")) {
    std::cerr << "Failed to load desert background" << std::endl;
  }
  desertTexture.setRepeated(true);

  if (!caveTexture.loadFromFile("assets/backgrounds/cave.jpg")) {
    std::cerr << "Failed to load cave background" << std::endl;
  }
  caveTexture.setRepeated(true);

  if (!mineshaftTexture.loadFromFile("assets/backgrounds/mineshaft.jpg")) {
    std::cerr << "Failed to load mineshaft background" << std::endl;
  }
  mineshaftTexture.setRepeated(true);
  
  loaded = true;
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
    selectedType = BackgroundType::Desert; 
  }

  currentType = selectedType;
  offset = 0.0f;

  const sf::Texture* tex = nullptr;
  switch (currentType) {
  case BackgroundType::Desert:
    tex = &desertTexture;
    break;
  case BackgroundType::Cave:
    tex = &caveTexture;
    break;
  case BackgroundType::Mineshaft:
    tex = &mineshaftTexture;

    {
        static std::random_device rd_mineshaft;
        static std::mt19937 gen_mineshaft(rd_mineshaft());
        std::uniform_int_distribution<> dis_mineshaft(0, 1);
        int idx = dis_mineshaft(gen_mineshaft);

    }
    break;
  }
  
  if (tex) {
      currentBackground1.emplace(*tex);
      currentBackground2.emplace(*tex);
  }
}

void GameBackground::update(float deltaTime) {
    if (!loaded) return;
    if (!currentBackground1.has_value()) return;
    
    offset += speed * deltaTime;
    float textureWidth = static_cast<float>(currentBackground1->getTexture().getSize().x);
    if (offset >= textureWidth) {
       offset -= textureWidth;
    }
}

void GameBackground::draw(sf::RenderWindow &window) const {
  if (!loaded || !currentBackground1.has_value() || !currentBackground2.has_value())
    return;

  auto winSize = window.getSize();
  auto texSize = currentBackground1->getTexture().getSize();

  float scaleY = static_cast<float>(winSize.y) / static_cast<float>(texSize.y);
  
  float textureWidth = static_cast<float>(texSize.x) * scaleY;

  currentBackground1->setScale({scaleY, scaleY});
  currentBackground1->setPosition({-offset * scaleY, 0.f});

  currentBackground2->setScale({scaleY, scaleY});
  currentBackground2->setPosition({textureWidth - offset * scaleY, 0.f});

  window.draw(*currentBackground1);
  window.draw(*currentBackground2);
}
