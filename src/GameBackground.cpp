#include "GameBackground.h"
#include <random>
#include "Exceptions.h"



GameBackground::GameBackground() : currentType(BackgroundType::Desert), offset(0.0f), speed(30.0f), loaded(false), ambientTimer(10.0f) {
  loadTextures();
  loadSounds();
}

void GameBackground::loadTextures() {
  if (!desertTexture.loadFromFile("assets/backgrounds/desert.jpg")) {
    throw AssetLoadException("assets/backgrounds/desert.jpg", "Texture");
  }
  desertTexture.setRepeated(true);

  if (!caveTexture.loadFromFile("assets/backgrounds/cave.jpg")) {
    throw AssetLoadException("assets/backgrounds/cave.jpg", "Texture");
  }
  caveTexture.setRepeated(true);

  if (!mineshaftTexture.loadFromFile("assets/backgrounds/mineshaft.jpg")) {
    throw AssetLoadException("assets/backgrounds/mineshaft.jpg", "Texture");
  }
  mineshaftTexture.setRepeated(true);
  
  if (!classicTexture.loadFromFile("assets/classic1.jpg")) {
      throw AssetLoadException("assets/classic1.jpg", "Texture");
  }
  classicTexture.setRepeated(true);
  
  loaded = true;
}

void GameBackground::loadSounds() {
  for (int i = 1; i <= 12; ++i) {
    sf::SoundBuffer buffer;
    std::string filename = "assets/sound/Cave" + std::to_string(i) + ".ogg";
    if (buffer.loadFromFile(filename)) {
      caveBuffers.push_back(buffer);
    } else {
        throw AssetLoadException(filename, "Sound");
    }
  }
}


void GameBackground::selectBackground(const GameConfig &config) {
  BackgroundType selectedType;

  if (config.betaStyle) {
      selectedType = BackgroundType::Classic;
  } else if (config.timeMode && config.spidersMode) {
    selectedType = BackgroundType::Mineshaft;
  } else if (config.torchMode || config.spidersMode) {
    selectedType = BackgroundType::Cave;
  } else {
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
    break;
  case BackgroundType::Classic:
    tex = &classicTexture;
    break;
  }
  
  if (tex) {
      currentBackground1.emplace(*tex);
      currentBackground2.emplace(*tex);
  }
}

void GameBackground::update(float deltaTime, sf::Vector2u windowSize, bool shouldScroll) {
  if (!loaded)
    return;
  if (!currentBackground1.has_value())
    return;

  if (shouldScroll) {
    offset += speed * deltaTime;
    float textureWidth =
        static_cast<float>(currentBackground1->getTexture().getSize().x);
    if (offset >= textureWidth) {
      offset -= textureWidth;
    }
  } else {

    auto texSize = currentBackground1->getTexture().getSize();
    float scaleY =
        static_cast<float>(windowSize.y) / static_cast<float>(texSize.y);
    float textureWidth = static_cast<float>(texSize.x);
    float availableSlack = textureWidth - (static_cast<float>(windowSize.x) / scaleY);
    if (currentType == BackgroundType::Classic) {
        offset = availableSlack * 0.70f;
    } else {
        offset = availableSlack / 2.0f;
    }
  }

  // Ambient sound update
  if (currentType == BackgroundType::Cave ||
      currentType == BackgroundType::Mineshaft) {
    ambientTimer -= deltaTime;
    if (ambientTimer <= 0.0f) {
      // Play random cave sound
      if (!caveBuffers.empty()) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0,
                                            static_cast<int>(caveBuffers.size()) -
                                                1);

        int idx = dis(gen);
        ambientSound.emplace(caveBuffers[idx]);
        ambientSound->setVolume(currentVolume);
        ambientSound->play();
        // std::cout << "Playing ambient sound index " << idx << std::endl;
      }

      // Result 1-3 mins (60-180s)
      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_real_distribution<float> timeDis(60.0f, 180.0f);
      ambientTimer = timeDis(gen);
    }
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
