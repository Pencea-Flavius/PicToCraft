#ifndef OOP_WINSCREEN_H
#define OOP_WINSCREEN_H

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

class WinScreen {
  sf::Font font;
  sf::Texture backgroundTexture;
  std::optional<sf::Sprite> backgroundSprite;
  sf::Texture logoTexture;
  std::optional<sf::Sprite> logoSprite;

  std::vector<std::string> paragraphs;
  std::vector<sf::Color> paragraphColors;

  float scrollOffset;
  float fadeAlpha;
  sf::Clock clock;

  float baseWidth = 1280.0f;
  float baseHeight = 720.0f;
  float contentHeight = 0.0f;

  void loadPoemText();
  sf::Vector2f calculateScale(const sf::RenderWindow &window) const;

public:
  WinScreen();
  ~WinScreen();

  void reset();
  void update(float deltaTime);
  void draw(sf::RenderWindow &window);
  bool isFinished() const;
};

#endif // OOP_WINSCREEN_H
