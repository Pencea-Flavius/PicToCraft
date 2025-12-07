#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

#include "GameMode.h"
#include <SFML/Graphics.hpp>
#include <optional>

enum class BackgroundType { Plains, Desert, Cave, Mineshaft };

class GameBackground {
public:
  GameBackground();

  void selectBackground(const GameConfig &config);
  void draw(sf::RenderWindow &window) const;

private:
  sf::Texture plainsTexture;
  sf::Texture desertTexture;
  sf::Texture caveTexture;
  sf::Texture mineshaftTexture;

  mutable std::optional<sf::Sprite> currentBackground;
  BackgroundType currentType;

  void loadTextures();
};

#endif // GAMEBACKGROUND_H
