#include "GridRenderer.h"
#include "ShadowedText.h"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>

GridRenderer::GridRenderer(Grid &g, float size, sf::Vector2f off)
  : grid(g), cellSize(size), offset(off), fontLoaded(false), lastMistakes(0),
    backgroundPatch(sf::Texture(), 4,
                    10),
    hintTabPatch(sf::Texture(), 4, 0) {
  fontLoaded = font.openFromFile("assets/Monocraft.ttf");
  if (!webTexture.loadFromFile("assets/cobweb.png")) {
    std::cerr << "Failed to load cobweb texture" << std::endl;
  }

  if (!backgroundTexture.loadFromFile("assets/grid/container.png")) {
    std::cerr << "Failed to load assets/grid/container.png" << std::endl;
  } else {
    backgroundPatch.setTexture(backgroundTexture);
    backgroundPatch.setPatchScale(
      1.0f);
  }

  if (!blockTexture.loadFromFile("assets/grid/slot.png")) {
    std::cerr << "Failed to load assets/grid/slot.png" << std::endl;
  }

  if (!hintTabTexture.loadFromFile("assets/grid/tab_hint.png")) {
    std::cerr << "Failed to load assets/grid/tab_hint.png" << std::endl;
  } else {
    hintTabPatch.setTexture(hintTabTexture);
    hintTabPatch.setCornerSize(4);
  }

  // Load break textures
  for (int i = 0; i <= 9; ++i) {
    sf::Texture tex;
    std::string path =
        "assets/break/destroy_stage_" + std::to_string(i) + ".png";
    if (!tex.loadFromFile(path)) {
      std::cerr << "Failed to load " << path << std::endl;
    }
    breakTextures.push_back(tex);
  }
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
      {
        winSize.x - bounds.size.x - bounds.position.x - padding,
        padding - bounds.position.y
      });

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

void GridRenderer::drawHintTabs(sf::RenderWindow &window) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
  float colHintsHeight = maxColHeight * cellSize * 0.8f;

  sf::Vector2f gridOffset = {
    offset.x + rowHintsWidth,
    offset.y + colHintsHeight
  };

  auto winSize = window.getSize();
  float scaleX = static_cast<float>(winSize.x) / 1280.0f;
  float scaleY = static_cast<float>(winSize.y) / 720.0f;
  float uiScale = std::min(scaleX, scaleY);

  int n = grid.get_size();
  float padding = 4.0f * uiScale;

  float tabWidth = cellSize - padding;

  // Sprite setup
  sf::Sprite capSprite(hintTabTexture);
  sf::Sprite bodySprite(hintTabTexture);

  int texW = hintTabTexture.getSize().x;
  int texH = hintTabTexture.getSize().y;
  int capH = 6; // Guessing
  int bodyH = texH - capH;

  capSprite.setTextureRect(sf::IntRect({0, 0}, {texW, capH}));
  bodySprite.setTextureRect(sf::IntRect({0, capH}, {texW, bodyH}));

  float spriteScaleX = tabWidth / static_cast<float>(texW);
  float slotHeight = cellSize * 0.8f;
  float spriteScaleY_Body = slotHeight / static_cast<float>(bodyH);
  float spriteScaleY_Cap =
      spriteScaleX;

  bodySprite.setScale({spriteScaleX, spriteScaleY_Body});
  capSprite.setScale({spriteScaleX, spriteScaleY_Cap});

  const auto &colHints = hints.get_col_hints();
  for (int j = 0; j < n; ++j) {
    float x = gridOffset.x + j * cellSize + padding / 2.0f;

    float bottomY = gridOffset.y - 8.0f * uiScale;

    size_t currentColHeight = colHints[j].size();
    if (currentColHeight == 0)
      currentColHeight = 1;

    for (size_t k = 0; k < currentColHeight; ++k) {
      float y = bottomY - (k + 1) * slotHeight;
      bodySprite.setPosition({x, y});
      window.draw(bodySprite);
    }

    float capY = bottomY - currentColHeight * slotHeight -
                 (static_cast<float>(capH) * spriteScaleY_Cap);

    capSprite.setPosition({x, capY});
    window.draw(capSprite);
  }

  // Draw Row Tabs (Left)
  bodySprite.setRotation(sf::degrees(-90.f));
  capSprite.setRotation(sf::degrees(-90.f));

  bodySprite.setOrigin({0.f, 0.f});
  capSprite.setOrigin({0.f, 0.f});

  const auto &rowHints = hints.get_row_hints();
  for (int i = 0; i < n; ++i) {
    float y = gridOffset.y + i * cellSize + padding / 2.0f;
    // Shifted left further as requested (was -8.0f, now -14.0f)
    float rightX = gridOffset.x - 14.0f * uiScale;

    size_t currentRowWidth = rowHints[i].size();
    if (currentRowWidth == 0)
      currentRowWidth = 1;

    for (size_t k = 0; k < currentRowWidth; ++k) {
      float visualBodyWidth = bodySprite.getGlobalBounds().size.x;
      float visualBodyHeight = bodySprite.getGlobalBounds().size.y;

      float slotX = rightX - (k + 1) * visualBodyWidth;
      float slotY = y + visualBodyHeight;

      bodySprite.setPosition({slotX, slotY});
      window.draw(bodySprite);
    }

    float visualCapWidth = capSprite.getGlobalBounds().size.x;
    float visualCapHeight = capSprite.getGlobalBounds().size.y;

    float visualBodyWidth = bodySprite.getGlobalBounds().size.x;
    float capX = rightX - currentRowWidth * visualBodyWidth - visualCapWidth;
    float capY = y + visualCapHeight;

    capSprite.setPosition({capX, capY});
    window.draw(capSprite);
  }

  bodySprite.setRotation(sf::degrees(0.f));
  capSprite.setRotation(sf::degrees(0.f));
  bodySprite.setOrigin({0.f, 0.f});
  capSprite.setOrigin({0.f, 0.f});
}

void GridRenderer::draw(sf::RenderWindow &window) const {
  const auto &hints = grid.get_hints();
  int n = grid.get_size();

  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
  float colHintsHeight = maxColHeight * cellSize * 0.8f;

  sf::Vector2f gridOffset = {
    offset.x + rowHintsWidth,
    offset.y + colHintsHeight
  };

  drawHintTabs(window);

  // Draw Background
  float gridWidth = n * cellSize;
  float gridHeight = n * cellSize;
  float padding = 16.0f;

  auto winSize = window.getSize();
  float scaleX = static_cast<float>(winSize.x) / 1280.0f;
  float scaleY = static_cast<float>(winSize.y) / 720.0f;
  float uiScale = std::min(scaleX, scaleY);

  float borderScale = 16.0f / 4.0f;
  float visualBorderScale = std::round(uiScale * borderScale);
  if (visualBorderScale < 1.0f)
    visualBorderScale = 1.0f;

  auto &mutablePatch = const_cast<NinePatch &>(backgroundPatch);
  mutablePatch.setPixelSnapping(true);
  mutablePatch.setPatchScale(visualBorderScale / borderScale);

  float scaledPadding = std::round(padding * uiScale);

  mutablePatch.setSize(gridWidth + scaledPadding * 2,
                       gridHeight + scaledPadding * 2);
  mutablePatch.setPosition(
    {gridOffset.x - scaledPadding, gridOffset.y - scaledPadding});
  window.draw(backgroundPatch);

  if (fontLoaded) {
    const auto &rowHints = hints.get_row_hints();
    for (size_t i = 0; i < rowHints.size(); ++i) {
      float rowY = gridOffset.y + i * cellSize + cellSize * 0.5f;
      float slotHeight = cellSize * 0.8f;

      for (int j = static_cast<int>(rowHints[i].size()) - 1; j >= 0; --j) {
        int k = rowHints[i].size() - 1 - j;

        float slotCenter =
            gridOffset.x - (k * slotHeight) - (slotHeight / 2.0f);
        slotCenter -= 16.0f * uiScale;
        unsigned int fontSize = static_cast<unsigned int>(
          std::min(28.f * uiScale, cellSize * 0.5f));

        if (grid.isHintWebbed(true, i, j)) {
          //Web logic
          sf::Sprite webSprite(webTexture);
          float scale =
              (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
          webSprite.setScale({scale, scale});
          webSprite.setOrigin(
            {
              static_cast<float>(webTexture.getSize().x) / 2.f,
              static_cast<float>(webTexture.getSize().y) / 2.f
            });

          sf::Text tempText(font, std::to_string(rowHints[i][j]));
          tempText.setCharacterSize(fontSize);
          auto bounds = tempText.getLocalBounds();

          float maxWidth = slotHeight * 0.9f;
          if (bounds.size.x > maxWidth) {
            float scaleFactor = maxWidth / bounds.size.x;
            tempText.setScale({scaleFactor, scaleFactor});
            bounds = tempText.getGlobalBounds(); // Update bounds
          }

          // Center text
          float textX =
              std::round(slotCenter - bounds.size.x / 2.0f - bounds.position.x);
          float textY =
              std::round(rowY - bounds.size.y * 0.5f - bounds.position.y);

          webSprite.setPosition({
            textX + bounds.size.x / 2.f,
            textY + bounds.size.y / 2.f + 2.0f * uiScale
          });
          window.draw(webSprite);

          // Draw break texture
          int health = grid.get_hints().getWebHealth(true, i, j);
          if (health < 10) {
            int stage = 9 - health; // 9 down to 0
            if (stage >= 0 && stage < static_cast<int>(breakTextures.size())) {
              sf::Sprite breakSprite(breakTextures[stage]);
              breakSprite.setScale({scale, scale});
              breakSprite.setOrigin(webSprite.getOrigin());
              breakSprite.setPosition(webSprite.getPosition());
              window.draw(breakSprite);
            }
          }
        } else {
          sf::Text text(font, std::to_string(rowHints[i][j]));
          text.setCharacterSize(fontSize);
          text.setFillColor(sf::Color::White);

          auto bounds = text.getLocalBounds();

          // Constrain width if too wide
          float maxWidth = slotHeight * 0.9f;
          if (bounds.size.x > maxWidth) {
            float scaleFactor = maxWidth / bounds.size.x;
            text.setScale({scaleFactor, scaleFactor});
            bounds = text.getGlobalBounds(); // Update bounds
          }

          float textX =
              std::round(slotCenter - bounds.size.x / 2.0f - bounds.position.x);
          float textY =
              std::round(rowY - bounds.size.y * 0.5f - bounds.position.y);

          text.setPosition({textX, textY});

          ShadowedText::draw(window, text, uiScale);
        }
      }
    }

    const auto &colHints = hints.get_col_hints();
    for (size_t j = 0; j < colHints.size(); ++j) {
      float colX = gridOffset.x + j * cellSize + cellSize * 0.5f;
      float slotHeight = cellSize * 0.8f;

      for (int i = static_cast<int>(colHints[j].size()) - 1; i >= 0; --i) {
        // k=0 is closest to grid (bottom).
        int k = colHints[j].size() - 1 - i;

        float slotCenterY =
            gridOffset.y - (k * slotHeight) - (slotHeight / 2.0f);
        slotCenterY -= 16.0f * uiScale;

        // Dynamic font size
        unsigned int fontSize = static_cast<unsigned int>(
          std::min(28.f * uiScale, cellSize * 0.5f));

        if (grid.isHintWebbed(false, j, i)) {
          sf::Sprite webSprite(webTexture);
          float scale =
              (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
          webSprite.setScale({scale, scale});
          webSprite.setOrigin(
            {
              static_cast<float>(webTexture.getSize().x) / 2.f,
              static_cast<float>(webTexture.getSize().y) / 2.f
            });

          sf::Text tempText(font, std::to_string(colHints[j][i]));
          tempText.setCharacterSize(fontSize);
          auto bounds = tempText.getLocalBounds();

          float maxWidth = (cellSize - padding) * 0.9f;
          if (bounds.size.x > maxWidth) {
            float scaleFactor = maxWidth / bounds.size.x;
            tempText.setScale({scaleFactor, scaleFactor});
            bounds = tempText.getGlobalBounds();
          }

          float textX =
              std::round(colX - bounds.size.x * 0.5f - bounds.position.x);
          float textY = std::round(slotCenterY - bounds.size.y * 0.5f -
                                   bounds.position.y);

          webSprite.setPosition({
            textX + bounds.size.x / 2.f,
            textY + bounds.size.y / 2.f + 4.0f * uiScale
          });
          window.draw(webSprite);

          // Draw break texture
          int health = grid.get_hints().getWebHealth(false, j, i);
          if (health < 10) {
            int stage = 9 - health; // 9 down to 0
            if (stage >= 0 && stage < static_cast<int>(breakTextures.size())) {
              sf::Sprite breakSprite(breakTextures[stage]);
              breakSprite.setScale({scale, scale});
              breakSprite.setOrigin(webSprite.getOrigin());
              breakSprite.setPosition(webSprite.getPosition());
              window.draw(breakSprite);
            }
          }
        } else {
          sf::Text text(font, std::to_string(colHints[j][i]));
          text.setCharacterSize(fontSize);
          text.setFillColor(sf::Color::White);

          auto bounds = text.getLocalBounds();

          // Constrain width if too wide
          float maxWidth = (cellSize - padding) * 0.9f;
          if (bounds.size.x > maxWidth) {
            float scaleFactor = maxWidth / bounds.size.x;
            text.setScale({scaleFactor, scaleFactor});
            bounds = text.getGlobalBounds();
          }

          float textX =
              std::round(colX - bounds.size.x * 0.5f - bounds.position.x);
          float textY = std::round(slotCenterY - bounds.size.y * 0.5f -
                                   bounds.position.y);

          text.setPosition({textX, textY});

          ShadowedText::draw(window, text, uiScale);
        }
      }
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      sf::Vector2f pos = {
        gridOffset.x + j * cellSize,
        gridOffset.y + i * cellSize
      };

      // Draw slot texture
      sf::Sprite slotSprite(blockTexture);
      float scaleX = cellSize / blockTexture.getSize().x;
      float scaleY = cellSize / blockTexture.getSize().y;
      slotSprite.setScale({scaleX, scaleY});
      slotSprite.setPosition(pos);
      window.draw(slotSprite);

      if (grid.get_block(i, j).is_completed()) {
        // Draw filled block (black square inside slot)
        float padding = cellSize * 0.15f;
        sf::RectangleShape rect(
          sf::Vector2f(cellSize - 2 * padding, cellSize - 2 * padding));
        rect.setPosition({pos.x + padding, pos.y + padding});
        rect.setFillColor(sf::Color::Black);
        window.draw(rect);
      }
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

  sf::Vector2f gridOffset = {
    offset.x + rowHintsWidth,
    offset.y + colHintsHeight};

  if (mousePos.x < gridOffset.x || mousePos.y < gridOffset.y)
    return;

  int n = grid.get_size();
  int x = static_cast<int>((mousePos.y - gridOffset.y) / cellSize);
  int y = static_cast<int>((mousePos.x - gridOffset.x) / cellSize);

  if (x >= 0 && x < n && y >= 0 && y < n)
    grid.toggle_block(x, y);
}

sf::Vector2f GridRenderer::getHintCenter(bool isRow, int line,
                                         int index) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
  float colHintsHeight = maxColHeight * cellSize * 0.8f;

  sf::Vector2f gridOffset = {
    offset.x + rowHintsWidth,
    offset.y + colHintsHeight
  };

  if (isRow) {
    const auto &rowHints = hints.get_row_hints();
    if (line >= rowHints.size() || index >= rowHints[line].size())
      return {0, 0};

    float rowY = gridOffset.y + line * cellSize + cellSize * 0.5f;
    float startX = gridOffset.x - 20.f;

    // Calculate X position based on index (reverse order)
    int reverseIndex = rowHints[line].size() - 1 - index;
    float currentX = startX - reverseIndex * (cellSize * 0.8f);

    sf::Text tempText(font, std::to_string(rowHints[line][index]));
    tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
    auto bounds = tempText.getLocalBounds();

    float textX = currentX - bounds.size.x - bounds.position.x;
    float textY = rowY - bounds.size.y * 0.5f - bounds.position.y;

    return {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f};
  } else {
    const auto &colHints = hints.get_col_hints();
    if (line >= colHints.size() || index >= colHints[line].size())
      return {0, 0};

    float colX = gridOffset.x + line * cellSize + cellSize * 0.5f;
    float startY = gridOffset.y - 10.f;

    int reverseIndex = colHints[line].size() - 1 - index;
    float currentY = startY - reverseIndex * (cellSize * 0.8f);

    sf::Text tempText(font, std::to_string(colHints[line][index]));
    tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
    auto bounds = tempText.getLocalBounds();

    float textX = colX - bounds.size.x * 0.5f - bounds.position.x;
    float textY = currentY - bounds.size.y - bounds.position.y;

    return {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f};
  }
}

void GridRenderer::handleHintClick(const sf::Vector2i &mousePos) const {
  const auto &hints = grid.get_hints();

  // Check all webbed hints to see if clicked
  const auto &rowHints = hints.get_row_hints();
  for (size_t i = 0; i < rowHints.size(); ++i) {
    for (size_t j = 0; j < rowHints[i].size(); ++j) {
      if (grid.isHintWebbed(true, i, j)) {
        sf::Vector2f center = getHintCenter(true, i, j);
        float radius = cellSize * 0.4f; // Approximate hit radius

        float dx = mousePos.x - center.x;
        float dy = mousePos.y - center.y;
        if (dx * dx + dy * dy <= radius * radius) {
          const_cast<Grid &>(grid).damageWeb(true, i, j); // Use damageWeb
          return; // Handle one click at a time
        }
      }
    }
  }

  const auto &colHints = hints.get_col_hints();
  for (size_t j = 0; j < colHints.size(); ++j) {
    for (size_t i = 0; i < colHints[j].size(); ++i) {
      if (grid.isHintWebbed(false, j, i)) {
        sf::Vector2f center = getHintCenter(false, j, i);
        float radius = cellSize * 0.4f;

        float dx = mousePos.x - center.x;
        float dy = mousePos.y - center.y;
        if (dx * dx + dy * dy <= radius * radius) {
          const_cast<Grid &>(grid).damageWeb(false, j, i); // Use damageWeb
          return;
        }
      }
    }
  }
}
