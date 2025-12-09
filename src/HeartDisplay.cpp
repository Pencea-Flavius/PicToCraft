#include "HeartDisplay.h"
#include "Exceptions.h"
#include <random>

HeartDisplay::HeartDisplay() : isFlashing(false), flashTimer(0.0f) {
  if (!containerTexture.loadFromFile("assets/hearts/container_hardcore.png")) {
    throw AssetLoadException("assets/hearts/container_hardcore.png", "Texture");
  }
  if (!containerBlinkingTexture.loadFromFile(
          "assets/hearts/container_hardcore_blinking.png")) {
    throw AssetLoadException("assets/hearts/container_hardcore_blinking.png",
                             "Texture");
  }

  if (!fullHeartTexture.loadFromFile("assets/hearts/hardcore_full.png")) {
    throw AssetLoadException("assets/hearts/hardcore_full.png", "Texture");
  }
  if (!fullHeartBlinkingTexture.loadFromFile(
          "assets/hearts/hardcore_full_blinking.png")) {
    throw AssetLoadException("assets/hearts/hardcore_full_blinking.png",
                             "Texture");
  }

  if (!halfHeartTexture.loadFromFile("assets/hearts/hardcore_half.png")) {
    throw AssetLoadException("assets/hearts/hardcore_half.png", "Texture");
  }
  if (!halfHeartBlinkingTexture.loadFromFile(
          "assets/hearts/hardcore_half_blinking.png")) {
    throw AssetLoadException("assets/hearts/hardcore_half_blinking.png",
                             "Texture");
  }

  if (!poisonedFullHeartTexture.loadFromFile(
          "assets/hearts/poisoned_hardcore_full.png")) {
    throw AssetLoadException("assets/hearts/poisoned_hardcore_full.png",
                             "Texture");
  }
  if (!poisonedFullHeartBlinkingTexture.loadFromFile(
          "assets/hearts/poisoned_hardcore_full_blinking.png")) {
    throw AssetLoadException(
        "assets/hearts/poisoned_hardcore_full_blinking.png", "Texture");
  }

  if (!poisonedHalfHeartTexture.loadFromFile(
          "assets/hearts/poisoned_hardcore_half.png")) {
    throw AssetLoadException("assets/hearts/poisoned_hardcore_half.png",
                             "Texture");
  }
  if (!poisonedHalfHeartBlinkingTexture.loadFromFile(
          "assets/hearts/poisoned_hardcore_half_blinking.png")) {
    throw AssetLoadException(
        "assets/hearts/poisoned_hardcore_half_blinking.png", "Texture");
  }

  containerSprite.emplace(containerTexture);
  fullHeartSprite.emplace(fullHeartTexture);
  halfHeartSprite.emplace(halfHeartTexture);
}

void HeartDisplay::update(float deltaTime) {
  if (isFlashing) {
    flashTimer -= deltaTime;
    if (flashTimer <= 0.0f) {
      isFlashing = false;
      flashTimer = 0.0f;
    }
  }
}

void HeartDisplay::triggerFlash() {
  isFlashing = true;
  flashTimer = FLASH_DURATION;
}

void HeartDisplay::draw(sf::RenderWindow &window, int currentMistakes,
                        int maxMistakes, const sf::Vector2f &position,
                        float scale, bool isPoisoned) {

  bool useBlinking =
      isFlashing && (static_cast<int>(flashTimer * 5.0f) % 2 == 0);

  if (useBlinking) {
    containerSprite->setTexture(containerBlinkingTexture);
    if (isPoisoned) {
      fullHeartSprite->setTexture(poisonedFullHeartBlinkingTexture);
      halfHeartSprite->setTexture(poisonedHalfHeartBlinkingTexture);
    } else {
      fullHeartSprite->setTexture(fullHeartBlinkingTexture);
      halfHeartSprite->setTexture(halfHeartBlinkingTexture);
    }
  } else {
    containerSprite->setTexture(containerTexture);
    if (isPoisoned) {
      fullHeartSprite->setTexture(poisonedFullHeartTexture);
      halfHeartSprite->setTexture(poisonedHalfHeartTexture);
    } else {
      fullHeartSprite->setTexture(fullHeartTexture);
      halfHeartSprite->setTexture(halfHeartTexture);
    }
  }

  // Scale sprites
  containerSprite->setScale({scale, scale});
  fullHeartSprite->setScale({scale, scale});
  halfHeartSprite->setScale({scale, scale});

  int totalHalfHearts = maxMistakes;
  int currentHalfHearts = std::max(0, maxMistakes - currentMistakes);

  int displayHalfHearts = currentHalfHearts;
  if (isFlashing && currentMistakes > 0) {
    displayHalfHearts = std::min(totalHalfHearts, currentHalfHearts + 1);
  }

  int totalHearts = (totalHalfHearts + 1) / 2;
  float heartSpacing = 9.0f * scale;

  bool isLowHealth =
      (static_cast<float>(currentHalfHearts) / totalHalfHearts) <= 0.2f ||
      currentHalfHearts <= 1;

  static std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  for (int i = 0; i < totalHearts; ++i) {
    float x = position.x + static_cast<float>(i) * heartSpacing;
    float y = position.y;

    if (isLowHealth) {
      float shakeOffset = dist(rng) * scale;
      y += shakeOffset;
    }

    containerSprite->setPosition({x, y});
    window.draw(*containerSprite);

    int heartValue = 0; // 0 = empty, 1 = half, 2 = full

    int firstHalfIndex = i * 2;
    int secondHalfIndex = i * 2 + 1;

    if (displayHalfHearts > secondHalfIndex) {
      heartValue = 2; // Full heart
    } else if (displayHalfHearts > firstHalfIndex) {
      heartValue = 1; // Half heart
    }

    if (heartValue == 2) {
      fullHeartSprite->setPosition({x, y});
      window.draw(*fullHeartSprite);
    } else if (heartValue == 1) {
      halfHeartSprite->setPosition({x, y});
      window.draw(*halfHeartSprite);
    }
  }
}
