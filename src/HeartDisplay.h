#ifndef OOP_HEARTDISPLAY_H
#define OOP_HEARTDISPLAY_H

#include <SFML/Graphics.hpp>
#include <optional>

class HeartDisplay {
  sf::Texture containerTexture;
  sf::Texture containerBlinkingTexture;
  sf::Texture fullHeartTexture;
  sf::Texture fullHeartBlinkingTexture;
  sf::Texture halfHeartTexture;
  sf::Texture halfHeartBlinkingTexture;

  sf::Texture poisonedFullHeartTexture;
  sf::Texture poisonedFullHeartBlinkingTexture;
  sf::Texture poisonedHalfHeartTexture;
  sf::Texture poisonedHalfHeartBlinkingTexture;
  
  sf::Texture witheredFullHeartTexture;
  sf::Texture witheredFullHeartBlinkingTexture;
  sf::Texture witheredHalfHeartTexture;
  sf::Texture witheredHalfHeartBlinkingTexture;

  std::optional<sf::Sprite> containerSprite;
  std::optional<sf::Sprite> fullHeartSprite;
  std::optional<sf::Sprite> halfHeartSprite;

  bool isFlashing;
  float flashTimer;
  const float FLASH_DURATION = 1.0f;
  
  float shakeTimer = 0.0f;
  const float SHAKE_DURATION = 0.25f;

public:
  HeartDisplay();

  void update(float deltaTime);
  void triggerFlash();
  void triggerShake();
  void draw(sf::RenderWindow &window, int currentMistakes, int maxMistakes,
            const sf::Vector2f &position, float scale, bool isPoisoned = false, bool isWithered = false);
};

#endif // OOP_HEARTDISPLAY_H
