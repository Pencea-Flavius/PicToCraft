#ifndef OOP_HEARTDISPLAY_H
#define OOP_HEARTDISPLAY_H

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

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

  std::optional<sf::Sprite> containerSprite;
  std::optional<sf::Sprite> fullHeartSprite;
  std::optional<sf::Sprite> halfHeartSprite;

  bool isFlashing;
  float flashTimer;
  const float FLASH_DURATION = 1.0f;

public:
  HeartDisplay();

  void update(float deltaTime);
  void triggerFlash();
  void draw(sf::RenderWindow &window, int currentMistakes, int maxMistakes,
            const sf::Vector2f &position, float scale, bool isPoisoned = false);
};

#endif // OOP_HEARTDISPLAY_H
