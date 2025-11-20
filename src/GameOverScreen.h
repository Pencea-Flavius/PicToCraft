#ifndef OOP_GAMEOVERSCREEN_H
#define OOP_GAMEOVERSCREEN_H

#include "MenuButton.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum class GameOverAction { None, Retry, MainMenu };

class GameOverScreen {
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreLabel;
    sf::Text scoreValue;
    sf::RectangleShape overlay;

    std::vector<std::unique_ptr<MenuButton>> buttons;

    void createButtons();
    sf::Vector2f calculateScale(const sf::RenderWindow &window) const;

    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;
public:
  GameOverScreen();
  ~GameOverScreen();

  void setScore(int score);
  GameOverAction handleEvent(const sf::Event &event,
                             const sf::RenderWindow &window);
  void update(const sf::RenderWindow &window);
  void draw(sf::RenderWindow &window);

};

#endif // OOP_GAMEOVERSCREEN_H
