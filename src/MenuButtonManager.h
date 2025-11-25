#ifndef OOP_MENUBUTTONMANAGER_H
#define OOP_MENUBUTTONMANAGER_H

#include "MenuButton.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class MenuButtonManager {
public:
  enum class LayoutType { MainMenu, GameSetup };

  MenuButtonManager(const sf::Font &font, const sf::Texture &buttonTexture,
                    const sf::Texture &buttonDisabledTexture);

  void createButtons(const std::vector<std::string> &labels,
                     unsigned int fontSize);

  void layoutMainMenu(const sf::RenderWindow &window, float scale,
                      float scaleY);
  void layoutGameSetup(const sf::RenderWindow &window, float scale,
                       float scaleY, int selectedTab, bool isTimeModeAvailable,
                       const sf::Texture &buttonTexture,
                       const sf::Texture &buttonDisabledTexture);

  int handleClick(const sf::Vector2f &mousePos) const;
  void update(const sf::Vector2f &mousePos);
  void draw(sf::RenderWindow &window);

  size_t getButtonCount() const { return buttons.size(); }

private:
  const sf::Font &font;
  const sf::Texture &buttonTexture;
  const sf::Texture &buttonDisabledTexture;

  std::vector<std::unique_ptr<MenuButton>> buttons;
};

#endif // OOP_MENUBUTTONMANAGER_H
