#ifndef OOP_MENUBUTTON_H
#define OOP_MENUBUTTON_H

#include <SFML/Graphics.hpp>
#include <string>

class MenuButton {
    sf::Sprite sprite;
    sf::Text text;
    bool hovered;

    unsigned int baseFontSize;
    float currentScale;
public:
  MenuButton(const std::string &label, const sf::Font &font,
             const sf::Texture &texture, unsigned int fontSize);
  ~MenuButton();

  // Updates the button's state (position, scale, hover)
  void update(const sf::RenderWindow &window, float scale, float x, float y,
              float widthScale, float heightScale,
              const sf::Vector2f &mousePos);

  void draw(sf::RenderWindow &window);
  bool isClicked(const sf::Vector2f &mousePos) const;

};

#endif // OOP_MENUBUTTON_H
