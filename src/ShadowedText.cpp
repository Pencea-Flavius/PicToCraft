#include "ShadowedText.h"
#include <cmath>

void ShadowedText::draw(sf::RenderWindow &window, const sf::Text &text,
                        const sf::Vector2f &position, float scale) {
  sf::Text shadow(text.getFont(), text.getString());
  shadow.setCharacterSize(text.getCharacterSize());
  shadow.setFillColor(sf::Color(0, 0, 0, 170));
  shadow.setRotation(text.getRotation());
  shadow.setOrigin(text.getOrigin());
  shadow.setScale(text.getScale());

  sf::Vector2f shadowOffset(std::round(2.0f * scale), std::round(2.0f * scale));
  sf::Vector2f snappedPos(std::round(position.x), std::round(position.y));

  shadow.setPosition(snappedPos + shadowOffset);
  window.draw(shadow);

  sf::Text mainText(text.getFont(), text.getString());
  mainText.setCharacterSize(text.getCharacterSize());
  mainText.setFillColor(text.getFillColor());
  mainText.setRotation(text.getRotation());
  mainText.setOrigin(text.getOrigin());
  mainText.setScale(text.getScale());
  mainText.setPosition(snappedPos);
  window.draw(mainText);
}

void ShadowedText::draw(sf::RenderWindow &window, const sf::Text &text,
                        float scale) {
  draw(window, text, text.getPosition(), scale);
}
