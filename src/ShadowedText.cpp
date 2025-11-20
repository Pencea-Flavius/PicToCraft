#include "ShadowedText.h"

void ShadowedText::draw(sf::RenderWindow &window, const sf::Text &text,
                        const sf::Vector2f &position, float scale) {
  sf::Text shadow(text.getFont(), text.getString());
  shadow.setCharacterSize(text.getCharacterSize());
  shadow.setFillColor(sf::Color(0, 0, 0, 170));
  shadow.setRotation(text.getRotation());
  shadow.setOrigin(text.getOrigin());
  shadow.setScale(text.getScale());
  shadow.setPosition(position + sf::Vector2f(2.0f * scale, 2.0f * scale));
  window.draw(shadow);

  sf::Text mainText(text.getFont(), text.getString());
  mainText.setCharacterSize(text.getCharacterSize());
  mainText.setFillColor(text.getFillColor());
  mainText.setRotation(text.getRotation());
  mainText.setOrigin(text.getOrigin());
  mainText.setScale(text.getScale());
  mainText.setPosition(position);
  window.draw(mainText);
}

void ShadowedText::draw(sf::RenderWindow &window, const sf::Text &text,
                        float scale) {
  sf::Text shadow(text.getFont(), text.getString());
  shadow.setCharacterSize(text.getCharacterSize());
  shadow.setFillColor(sf::Color(0, 0, 0, 170));
  shadow.setRotation(text.getRotation());
  shadow.setOrigin(text.getOrigin());
  shadow.setScale(text.getScale());
  shadow.setPosition(text.getPosition() +
                     sf::Vector2f(2.0f * scale, 2.0f * scale));
  window.draw(shadow);
  window.draw(text);
}
