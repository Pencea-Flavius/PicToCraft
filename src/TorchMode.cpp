#include "TorchMode.h"
#include <cmath>

TorchMode::TorchMode(std::unique_ptr<GameMode> mode)
    : GameModeDecorator(std::move(mode)) {}

bool TorchMode::isTorchMode() const { return true; }

std::unique_ptr<GameMode> TorchMode::clone() const {
  auto clonedWrapped = wrappedMode ? wrappedMode->clone() : nullptr;
  auto newMode = std::make_unique<TorchMode>(std::move(clonedWrapped));
  newMode->mistakes = this->mistakes;
  newMode->score = this->score;
  return newMode;
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

  sf::BlendMode subtractAlpha(
      sf::BlendMode::Factor::Zero, sf::BlendMode::Factor::One,
      sf::BlendMode::Equation::Add, sf::BlendMode::Factor::One,
      sf::BlendMode::Factor::One, sf::BlendMode::Equation::ReverseSubtract);

  lightLayer.draw(*lightSprite, subtractAlpha);
  lightLayer.display();

  sf::Sprite overlay(lightLayer.getTexture());
  sf::View originalView = window.getView();
  window.setView(window.getDefaultView());
  window.draw(overlay);
  window.setView(originalView);
}
