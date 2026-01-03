#ifndef OOP_PAUSEMENU_H
#define OOP_PAUSEMENU_H

#include "MenuButton.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>

enum class PauseAction { None, Resume, MainMenu };

class PauseMenu {
  sf::Font font;
  sf::Text titleText;
  sf::Texture pauseTexture;
  std::optional<sf::Sprite> pauseSprite;

  std::vector<std::unique_ptr<MenuButton>> buttons;

  float baseWidth = 1280.0f;
  float baseHeight = 720.0f;

  void createButtons();
  sf::Vector2f calculateScale(const sf::RenderWindow &window) const;

public:
  PauseMenu();
  ~PauseMenu() = default;

  PauseAction handleEvent(const sf::Event &event,
                          const sf::RenderWindow &window) const;
  void update(const sf::RenderWindow &window);
  void draw(sf::RenderWindow &window) const;
};

#endif // OOP_PAUSEMENU_H
