#include "GridRenderer.h"
#include "ShadowedText.h"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sstream>

GridRenderer::GridRenderer(Grid &g, float size, sf::Vector2f off)
    : grid(g), cellSize(size), offset(off), fontLoaded(false), lastMistakes(0) {
  fontLoaded = font.openFromFile("assets/Monocraft.ttf");
}

void GridRenderer::drawGameInfo(sf::RenderWindow &window) const {
  if (!fontLoaded)
    return;

  auto winSize = window.getSize();

  unsigned int fontSize = static_cast<unsigned int>(winSize.y * 0.035f);
  fontSize = std::max(18u, std::min(fontSize, 40u));

  sf::Text infoText(font, "");
  infoText.setCharacterSize(fontSize);
  infoText.setFillColor(sf::Color::Black);

  if (grid.shouldDisplayScore()) {
    std::ostringstream oss;
    oss << "Scor: " << grid.get_score();
    infoText.setString(oss.str());

    float padding = winSize.x * 0.015f;
    padding = std::max(10.f, std::min(padding, 30.f));

    auto bounds = infoText.getLocalBounds();
    infoText.setPosition(
        {winSize.x - bounds.size.x - bounds.position.x - padding,
         padding - bounds.position.y});

    float scaleX = static_cast<float>(winSize.x) / 1280.0f;
    float scaleY = static_cast<float>(winSize.y) / 720.0f;
    float scale = std::min(scaleX, scaleY);

    ShadowedText::draw(window, infoText, scale);
  } else {
    int currentMistakes = grid.get_mistakes();
    if (grid.get_mistakes() > lastMistakes) {
      heartDisplay.triggerFlash();
    }
    lastMistakes = grid.get_mistakes();

    float scaleX = static_cast<float>(winSize.x) / 1280.0f;
    float scaleY = static_cast<float>(winSize.y) / 720.0f;
    float baseScale = std::min(scaleX, scaleY);

    float heartScale = baseScale * 2.5f; // Make hearts 2.5x larger

    float padding = winSize.x * 0.015f;
    padding = std::max(10.f, std::min(padding, 30.f));

    int maxMistakes = grid.get_max_mistakes();
    int totalHearts = (maxMistakes + 1) / 2;
    float heartWidth = 9.0f * heartScale;
    float totalWidth = totalHearts * heartWidth;

    sf::Vector2f pos(winSize.x - totalWidth - padding, padding);

    heartDisplay.update(animationClock.restart().asSeconds());
    heartDisplay.draw(window, currentMistakes, maxMistakes, pos, heartScale,
                      grid.is_time_mode());
  }
}

void GridRenderer::draw(sf::RenderWindow &window) const {
  const auto &hints = grid.get_hints();
  int n = grid.get_size();

  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
  float colHintsHeight = maxColHeight * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  if (fontLoaded) {
    const auto &rowHints = hints.get_row_hints();
    for (size_t i = 0; i < rowHints.size(); ++i) {
      float rowY = gridOffset.y + i * cellSize + cellSize * 0.5f;
      float startX = gridOffset.x - 10.f;

      for (int j = static_cast<int>(rowHints[i].size()) - 1; j >= 0; --j) {
        sf::Text text(font, std::to_string(rowHints[i][j]));
        text.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
        text.setFillColor(sf::Color::Black);

        auto bounds = text.getLocalBounds();
        text.setPosition({startX - bounds.size.x - bounds.position.x,
                          rowY - bounds.size.y * 0.5f - bounds.position.y});

        window.draw(text);
        startX -= cellSize * 0.8f;
      }
    }

    const auto &colHints = hints.get_col_hints();
    for (size_t j = 0; j < colHints.size(); ++j) {
      float colX = gridOffset.x + j * cellSize + cellSize * 0.5f;
      float startY = gridOffset.y - 10.f;

      for (int i = static_cast<int>(colHints[j].size()) - 1; i >= 0; --i) {
        sf::Text text(font, std::to_string(colHints[j][i]));
        text.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
        text.setFillColor(sf::Color::Black);

        auto bounds = text.getLocalBounds();
        text.setPosition({colX - bounds.size.x * 0.5f - bounds.position.x,
                          startY - bounds.size.y - bounds.position.y});

        window.draw(text);
        startY -= cellSize * 0.8f;
      }
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      sf::RectangleShape rect(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
      rect.setPosition(
          {gridOffset.x + j * cellSize, gridOffset.y + i * cellSize});

      if (grid.get_block(i, j).is_completed())
        rect.setFillColor(sf::Color::Black);
      else
        rect.setFillColor(sf::Color::White);

      rect.setOutlineColor(sf::Color(180, 180, 180));
      rect.setOutlineThickness(1.f);
      window.draw(rect);
    }
  }

  grid.drawMode(window);
}

void GridRenderer::handleClick(const sf::Vector2i &mousePos) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
  float colHintsHeight = maxColHeight * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  int n = grid.get_size();
  int x = static_cast<int>((mousePos.y - gridOffset.y) / cellSize);
  int y = static_cast<int>((mousePos.x - gridOffset.x) / cellSize);

  if (x >= 0 && x < n && y >= 0 && y < n)
    grid.toggle_block(x, y);
}
