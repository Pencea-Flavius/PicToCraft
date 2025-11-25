#include "CustomCursor.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

CustomCursor::CustomCursor(sf::Window &window)
    : window(window), isEnabled(true), currentScale(0.2f), hotspotIdle(20, 100),
      hotspotClick(20, 100) {

  cursorIdle = loadCursor("assets/mouse/idle.png", hotspotIdle);
  cursorClick = loadCursor("assets/mouse/click.png", hotspotClick);
  cursorIdleTorch = loadCursor("assets/mouse/idle_torta.png", hotspotIdle);
  cursorClickTorch = loadCursor("assets/mouse/click_torta.png", hotspotClick);
  cursorSystem = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

  if (cursorIdle) {
    window.setMouseCursor(*cursorIdle);
  }
}

void CustomCursor::setScale(float scale) {
  currentScale = scale;
  cursorIdle = loadCursor("assets/mouse/idle.png", hotspotIdle);
  cursorClick = loadCursor("assets/mouse/click.png", hotspotClick);
  cursorIdleTorch = loadCursor("assets/mouse/idle_torta.png", hotspotIdle);
  cursorClickTorch = loadCursor("assets/mouse/click_torta.png", hotspotClick);

  if (isEnabled) {
    if (torchMode && cursorIdleTorch) {
      window.setMouseCursor(*cursorIdleTorch);
    } else if (!torchMode && cursorIdle) {
      window.setMouseCursor(*cursorIdle);
    }
  }
}

void CustomCursor::setTorchMode(bool enabled) {
  torchMode = enabled;
  if (isEnabled) {
    if (torchMode) {
      if (cursorIdleTorch)
        window.setMouseCursor(*cursorIdleTorch);
    } else {
      if (cursorIdle)
        window.setMouseCursor(*cursorIdle);
    }
  }
}

void CustomCursor::setEnabled(bool enabled) {
  isEnabled = enabled;
  if (isEnabled) {
    if (cursorIdle)
      window.setMouseCursor(*cursorIdle);
  } else {
    if (cursorSystem)
      window.setMouseCursor(*cursorSystem);
  }
}

void CustomCursor::handleEvent(const sf::Event &event) const {
  if (!isEnabled)
    return;

  if (event.is<sf::Event::MouseButtonPressed>()) {
    auto m = event.getIf<sf::Event::MouseButtonPressed>();
    if (m && m->button == sf::Mouse::Button::Left) {
      if (torchMode) {
        if (cursorClickTorch)
          window.setMouseCursor(*cursorClickTorch);
      } else {
        if (cursorClick)
          window.setMouseCursor(*cursorClick);
      }
    }
  } else if (event.is<sf::Event::MouseButtonReleased>()) {
    auto m = event.getIf<sf::Event::MouseButtonReleased>();
    if (m && m->button == sf::Mouse::Button::Left) {
      if (torchMode) {
        if (cursorIdleTorch)
          window.setMouseCursor(*cursorIdleTorch);
      } else {
        if (cursorIdle)
          window.setMouseCursor(*cursorIdle);
      }
    }
  }
}

std::optional<sf::Cursor> CustomCursor::loadCursor(const std::string &path,
                                                   sf::Vector2i hotspot) {
  sf::Image img;
  if (!img.loadFromFile(path))
    return std::nullopt;

  if (currentScale != 1.0f || hotspot.x < 0 || hotspot.y < 0) {
    sf::Texture tex;
    if (!tex.loadFromImage(img))
      return std::nullopt;
    tex.setSmooth(true);

    sf::Sprite sprite(tex);
    sprite.setScale({currentScale, currentScale});

    int paddingX = (hotspot.x < 0) ? -hotspot.x : 0;
    int paddingY = (hotspot.y < 0) ? -hotspot.y : 0;

    sprite.setPosition(
        {static_cast<float>(paddingX), static_cast<float>(paddingY)});

    unsigned int newWidth =
        static_cast<unsigned int>(img.getSize().x * currentScale) + paddingX;
    unsigned int newHeight =
        static_cast<unsigned int>(img.getSize().y * currentScale) + paddingY;

    sf::RenderTexture rt;
    if (!rt.resize({newWidth, newHeight}))
      return std::nullopt;

    rt.clear(sf::Color::Transparent);
    rt.draw(sprite);
    rt.display();
    img = rt.getTexture().copyToImage();

    hotspot.x += paddingX;
    hotspot.y += paddingY;
  }

  unsigned int hx = std::min(static_cast<unsigned int>(std::max(0, hotspot.x)),
                             img.getSize().x - 1);
  unsigned int hy = std::min(static_cast<unsigned int>(std::max(0, hotspot.y)),
                             img.getSize().y - 1);

  return sf::Cursor::createFromPixels(img.getPixelsPtr(), img.getSize(),
                                      {hx, hy});
}
