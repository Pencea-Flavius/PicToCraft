#ifndef OOP_SPLASHTEXT_H
#define OOP_SPLASHTEXT_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class SplashText {
public:
  SplashText();
  ~SplashText() = default;

  void update(float deltaTime);
  void draw(sf::RenderWindow &window, const sf::Sprite &titleSprite,
            float scale, float scaleY, float logoOriginalWidth,
            float logoOriginalHeight);

private:
  void loadMessages();
  void pickRandomMessage();

  sf::Font font;
  sf::Text text;
  std::vector<std::string> messages;

  bool increasing;
  float currentScale;
  float speed;
};

#endif // OOP_SPLASHTEXT_H
