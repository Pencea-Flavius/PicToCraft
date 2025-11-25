#ifndef OOP_MENUBUTTON_H
#define OOP_MENUBUTTON_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <string>

class MenuButton {
public:
  enum class Style { Default, Tab };

  MenuButton(const std::string &label, const sf::Font &font,
             const sf::Texture &texture, unsigned int fontSize);
  ~MenuButton();

  void update(float scale, float x, float y, float widthScale,
              float heightScale, const sf::Vector2f &mousePos);

  void draw(sf::RenderWindow &window);
  bool isClicked(const sf::Vector2f &mousePos) const;
  void setColor(const sf::Color &color);

  void setStyle(Style style);
  void setSelected(bool selected);
  void setTexture(const sf::Texture &texture);
  void setEnabled(bool enabled);

private:
  struct NinePatchConfig {
    int cornerSize;
    int edgeThickness;
  };

  static constexpr NinePatchConfig DEFAULT_NINE_PATCH = {4, 10};

  static sf::SoundBuffer clickSoundBuffer;
  static std::unique_ptr<sf::Sound> clickSound;
  static bool soundInitialized;

  std::vector<sf::Sprite> patchSprites;
  const sf::Texture *currentTexture;
  NinePatchConfig ninePatchConfig;

  sf::Text text;
  bool hovered;

  unsigned int baseFontSize;
  float currentScale;

  Style style = Style::Default;
  bool selected = false;
  bool enabled = true;

  sf::Vector2f buttonSize;
  sf::Vector2f position;

  void setupNinePatch();
  void updateNinePatchLayout();
};

#endif // OOP_MENUBUTTON_H
