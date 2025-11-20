#ifndef OOP_SHADOWEDTEXT_H
#define OOP_SHADOWEDTEXT_H

#include <SFML/Graphics.hpp>

class ShadowedText {
public:
  static void draw(sf::RenderWindow &window, const sf::Text &text,
                   const sf::Vector2f &position, float scale);

  static void draw(sf::RenderWindow &window, const sf::Text &text, float scale);
};

#endif // OOP_SHADOWEDTEXT_H
