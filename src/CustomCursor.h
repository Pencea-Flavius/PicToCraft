#ifndef CUSTOM_CURSOR_H
#define CUSTOM_CURSOR_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <string>

class CustomCursor {
public:
  explicit CustomCursor(sf::Window &window);

  void handleEvent(const sf::Event &event) const;
  void setScale(float scale);
  void setEnabled(bool enabled);
  void setTorchMode(bool enabled);

private:
  sf::Window &window;
  bool isEnabled;
  bool torchMode = false;
  float currentScale;

  sf::Vector2i hotspotIdle;
  sf::Vector2i hotspotClick;

  std::optional<sf::Cursor> cursorIdle;
  std::optional<sf::Cursor> cursorClick;
  std::optional<sf::Cursor> cursorIdleTorch;
  std::optional<sf::Cursor> cursorClickTorch;
  std::optional<sf::Cursor> cursorSystem;

  std::optional<sf::Cursor> loadCursor(const std::string &path,
                                       sf::Vector2i hotspot);
};

#endif // CUSTOM_CURSOR_H
