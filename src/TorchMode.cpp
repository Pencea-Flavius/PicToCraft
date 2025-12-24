#include "TorchMode.h"
#include "AlchemyMode.h"
#include "Exceptions.h"
#include <random>

TorchMode::TorchMode(std::unique_ptr<GameMode> mode)
    : GameModeDecorator(std::move(mode)), fireSound(dummyBuffer),
      silenceTimer(0), inSilence(false) {
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile("assets/sound/fire1.mp3")) {
    throw AssetLoadException("assets/sound/fire1.mp3", "Sound");
  }
  fireBuffers.push_back(buffer);
  if (!buffer.loadFromFile("assets/sound/fire2.mp3")) {
    throw AssetLoadException("assets/sound/fire2.mp3", "Sound");
  }
  fireBuffers.push_back(buffer);
  if (!buffer.loadFromFile("assets/sound/fire3.mp3")) {
    throw AssetLoadException("assets/sound/fire3.mp3", "Sound");
  }
  fireBuffers.push_back(buffer);

  playNextFireSound();
}

std::unique_ptr<GameMode> TorchMode::clone() const {
  auto clonedWrapped = wrappedMode ? wrappedMode->clone() : nullptr;
  auto newMode = std::make_unique<TorchMode>(std::move(clonedWrapped));
  newMode->mistakes = this->mistakes;
  newMode->score = this->score;
  return newMode;
}

void TorchMode::update(float deltaTime) {
  GameModeDecorator::update(deltaTime);

  if (inSilence) {
    silenceTimer += deltaTime;
    if (silenceTimer >= 3.0f) {
      inSilence = false;
      silenceTimer = 0.0f;
      playNextFireSound();
    }
  } else {
    if (fireSound.getStatus() == sf::SoundSource::Status::Stopped) {
      inSilence = true;
    }
  }

  particleSystem.update(deltaTime);
}

void TorchMode::playNextFireSound() {
  if (fireBuffers.empty())
    return;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,
                                      static_cast<int>(fireBuffers.size()) - 1);

  int index = dis(gen);
  fireSound.setBuffer(fireBuffers[index]);
  fireSound.setVolume(100.0f); // Increased volume
  fireSound.play();
}

void TorchMode::createLightTexture() const {
  unsigned int size = 500;
  sf::Image image;
  image.resize({size, size}, sf::Color::Transparent);

  float centerX = static_cast<float>(size) / 2.0f;
  float centerY = static_cast<float>(size) / 2.0f;
  float radius = static_cast<float>(size) / 2.0f;

  for (unsigned int y = 0; y < size; ++y) {
    for (unsigned int x = 0; x < size; ++x) {
      float dx = static_cast<float>(x) - centerX;
      float dy = static_cast<float>(y) - centerY;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance <= radius) {
        float alpha = 255.0f * (1.0f - distance / radius);
        image.setPixel(
            {x, y}, sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
      }
    }
  }

  (void)lightTexture.loadFromImage(image);
  lightTexture.setSmooth(true);
  lightTextureCreated = true;
}

void TorchMode::draw(sf::RenderWindow &window) const {
  if (!lightTextureCreated) {
    createLightTexture();
  }

  if (wrappedMode) {
    wrappedMode->draw(window);
  }
  
  // Check for Blindness/Night Vision from AlchemyMode in wrapped chain
  bool hasNightVision = false;
  
  if (wrappedMode) {
    // Try to get AlchemyMode by traversing decorator chain
    GameMode* current = wrappedMode.get();
    while (current) {
      if (const auto* alchemyMode = dynamic_cast<const AlchemyMode*>(current)) {
        hasNightVision = alchemyMode->hasEffect(EffectType::NightVision);
        break;
      }
      if (const auto* decorator = dynamic_cast<const GameModeDecorator*>(current)) {
        current = decorator->getWrappedMode();
      } else {
        break;
      }
    }
  }
  
  // Night Vision disables darkness completely
  if (hasNightVision) {
    // Draw particles but no darkness
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());
    
    sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
    auto mousePosF = static_cast<sf::Vector2f>(mousePosI);
    sf::Vector2f offset = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) 
                          ? sf::Vector2f(8.f, -48.f) 
                          : sf::Vector2f(15.f, -40.f);
    sf::Vector2f emitPos = mousePosF + offset;
    
    for (int i = 0; i < 5; ++i) {
      const_cast<TorchMode *>(this)->particleSystem.emit(emitPos, ParticleType::Fire);
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    if (dis(gen) == 0) {
      const_cast<TorchMode *>(this)->particleSystem.emit(emitPos, ParticleType::Smoke);
    }
    
    const_cast<TorchMode *>(this)->particleSystem.draw(window);
    window.setView(originalView);
    return; // Skip darkness rendering
  }


  // Show darkness (torch off or blindness)
  sf::Vector2u windowSize = window.getSize();
  if (lightLayer.getSize() != windowSize) {
    (void)lightLayer.resize(windowSize);
  }

  lightLayer.clear(sf::Color(0, 0, 0, 255));

  if (!lightSprite) {
    lightSprite.emplace(lightTexture);
  }
  lightSprite->setOrigin({static_cast<float>(lightTexture.getSize().x) / 2.0f,
                          static_cast<float>(lightTexture.getSize().y) / 2.0f});

  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  lightSprite->setPosition(static_cast<sf::Vector2f>(mousePos));

  float scale = static_cast<float>(window.getSize().x) / 1920.0f;
  lightSprite->setScale({scale, scale});

  sf::BlendMode subtractAlpha(
      sf::BlendMode::Factor::Zero, sf::BlendMode::Factor::One,
      sf::BlendMode::Equation::Add, sf::BlendMode::Factor::One,
      sf::BlendMode::Factor::One, sf::BlendMode::Equation::ReverseSubtract);

  lightSprite->setColor(sf::Color::White);
  lightLayer.draw(*lightSprite, subtractAlpha);

  sf::BlendMode addColor(sf::BlendMode::Factor::SrcAlpha,
                         sf::BlendMode::Factor::One,
                         sf::BlendMode::Equation::Add);

  lightSprite->setColor(sf::Color(255, 150, 50, 100));
  lightLayer.draw(*lightSprite, addColor);

  lightLayer.display();

  sf::Sprite overlay(lightLayer.getTexture());
  sf::View originalView = window.getView();
  window.setView(window.getDefaultView());
  window.draw(overlay);

  sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
  auto mousePosF = static_cast<sf::Vector2f>(mousePosI);

  sf::Vector2f offset = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)
                        ? sf::Vector2f(8.f, -48.f)
                        : sf::Vector2f(15.f, -40.f);

  sf::Vector2f emitPos = mousePosF + offset;

  for (int i = 0; i < 5; ++i) {
    const_cast<TorchMode *>(this)->particleSystem.emit(emitPos, ParticleType::Fire);
  }
  
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 1);
  if (dis(gen) == 0) {
    const_cast<TorchMode *>(this)->particleSystem.emit(emitPos, ParticleType::Smoke);
  }

  const_cast<TorchMode *>(this)->particleSystem.draw(window);

  window.setView(originalView);
}
