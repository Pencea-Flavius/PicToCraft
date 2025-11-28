#include "TorchMode.h"
#include "Exceptions.h"
#include <iostream>
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
  std::uniform_int_distribution<> dis(0, fireBuffers.size() - 1);

  int index = dis(gen);
  fireSound.setBuffer(fireBuffers[index]);
  fireSound.setVolume(100.0f); // Increased volume
  fireSound.play();
}

void TorchMode::createLightTexture() const {
  unsigned int size = 500;
  sf::Image image;
  image.resize({size, size}, sf::Color::Transparent);

  float centerX = size / 2.0f;
  float centerY = size / 2.0f;
  float radius = size / 2.0f;

  for (unsigned int y = 0; y < size; ++y) {
    for (unsigned int x = 0; x < size; ++x) {
      float dx = x - centerX;
      float dy = y - centerY;
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

  sf::Vector2u windowSize = window.getSize();
  if (lightLayer.getSize() != windowSize) {
    (void)lightLayer.resize(windowSize);
  }

  lightLayer.clear(sf::Color(0, 0, 0, 255));

  if (!lightSprite) {
    lightSprite.emplace(lightTexture);
  }
  lightSprite->setOrigin(
      {lightTexture.getSize().x / 2.0f, lightTexture.getSize().y / 2.0f});

  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  // Use screen coordinates for the light on the overlay
  lightSprite->setPosition(static_cast<sf::Vector2f>(mousePos));

  // Scale light based on resolution
  float scale = static_cast<float>(window.getSize().x) / 1920.0f;
  lightSprite->setScale({scale, scale});

  sf::BlendMode subtractAlpha(
      sf::BlendMode::Factor::Zero, sf::BlendMode::Factor::One,
      sf::BlendMode::Equation::Add, sf::BlendMode::Factor::One,
      sf::BlendMode::Factor::One, sf::BlendMode::Equation::ReverseSubtract);

  lightSprite->setColor(sf::Color::White); // Reset color for alpha subtraction
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

  // Emit particles at mouse position
  sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
  sf::Vector2f mousePosF = static_cast<sf::Vector2f>(mousePosI);

  float scaleFactor = static_cast<float>(window.getSize().x) / 1920.0f;

  sf::Vector2f offset(0.f, 0.f);

  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
    // Clicked position
    offset = sf::Vector2f(20.f, -60.f) * scaleFactor;
  } else {
    // Normal position
    // Adjusted for better alignment
    offset = sf::Vector2f(40.f, -90.f) * scaleFactor;
  }

  sf::Vector2f emitPos = mousePosF + offset;

  // Emit multiple particles for denser effect
  // Fire
  for (int i = 0; i < 5; ++i) {
    const_cast<TorchMode *>(this)->particleSystem.emit(emitPos,
                                                       ParticleType::Fire);
  }
  // Smoke (less frequent)
  if (rand() % 2 == 0) {
    const_cast<TorchMode *>(this)->particleSystem.emit(emitPos,
                                                       ParticleType::Smoke);
  }

  const_cast<TorchMode *>(this)->particleSystem.draw(window);

  window.setView(originalView);
}
