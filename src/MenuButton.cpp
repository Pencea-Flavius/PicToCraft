#include "MenuButton.h"
#include "ShadowedText.h"

MenuButton::MenuButton(const std::string &label, const sf::Font &font,
                       const sf::Texture &texture, unsigned int fontSize)
    : sprite(texture), text(font, label, fontSize), hovered(false),
      baseFontSize(fontSize), currentScale(1.0f) {
  auto bounds = sprite.getLocalBounds();
  sprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

  text.setFillColor(sf::Color::White);
}

MenuButton::~MenuButton() = default;

void MenuButton::update(const sf::RenderWindow &window, float scale, float x,
                        float y, float widthScale, float heightScale,
                        const sf::Vector2f &mousePos) {
  currentScale = scale;

  sprite.setScale({scale * widthScale, scale * heightScale});
  sprite.setPosition({x, y});

  // Check hover
  hovered = sprite.getGlobalBounds().contains(mousePos);
}

bool MenuButton::isClicked(const sf::Vector2f &mousePos) const {
  return sprite.getGlobalBounds().contains(mousePos);
}

void MenuButton::draw(sf::RenderWindow &window) {
  if (hovered) {
    sprite.setColor(sf::Color(160, 190, 240));
  } else {
    sprite.setColor(sf::Color::White);
  }
  window.draw(sprite);

  text.setCharacterSize(static_cast<unsigned int>(baseFontSize * currentScale));

  auto textBounds = text.getLocalBounds();
  float textX =
      sprite.getPosition().x - textBounds.size.x / 2.0f - textBounds.position.x;
  float textY =
      sprite.getPosition().y - textBounds.size.y / 2.0f - textBounds.position.y;

  if (hovered) {
    text.setFillColor(sf::Color(255, 255, 160));
  } else {
    text.setFillColor(sf::Color(221, 221, 221));
  }
  text.setPosition({textX, textY});

  ShadowedText::draw(window, text, currentScale);
}
