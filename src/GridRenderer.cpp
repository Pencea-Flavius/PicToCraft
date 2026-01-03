#include "GridRenderer.h"
#include "Exceptions.h"
#include "ShadowedText.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>

GridRenderer::GridRenderer(Grid &g, float size, sf::Vector2f off)
    : grid(g), cellSize(size), offset(off), lastMistakes(0),
      animationClock(), backgroundPatch(sf::Texture(), 4, 10),
      hintTabPatch(sf::Texture(), 4, 0), isDiscoFeverMode(false),
      defaultGlassColorIndex(0), colorTimer(0.0f), currentColorOffset(0) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    throw AssetLoadException("assets/Monocraft.ttf", "Font");
  }
  if (!webTexture.loadFromFile("assets/cobweb.png")) {
    throw AssetLoadException("assets/cobweb.png", "Texture");
  }

  if (!backgroundTexture.loadFromFile("assets/grid/container.png")) {
    throw AssetLoadException("assets/grid/container.png", "Texture");
  }
  backgroundPatch.setTexture(backgroundTexture);
  backgroundPatch.setPatchScale(1.0f);

  if (!blockTexture.loadFromFile("assets/grid/slot.png")) {
    throw AssetLoadException("assets/grid/slot.png", "Texture");
  }



  if (!hintTabTexture.loadFromFile("assets/grid/tab_hint.png")) {
    throw AssetLoadException("assets/grid/tab_hint.png", "Texture");
  }
  hintTabPatch.setTexture(hintTabTexture);
  hintTabPatch.setCornerSize(4);

  // Load break textures
  for (int i = 0; i <= 9; ++i) {
    sf::Texture tex;
    std::string path =
        "assets/break/destroy_stage_" + std::to_string(i) + ".png";
    if (!tex.loadFromFile(path)) {
      throw AssetLoadException(path, "Break Texture");
    }
    breakTextures.push_back(tex);
  }

  // Load glass textures
  std::vector<std::string> glassColors = {
      "black",      "blue",       "brown", "cyan",    "gray",   "green",
      "light_blue", "light_gray", "lime",  "magenta", "orange", "pink",
      "purple",     "red",        "white", "yellow"};

  for (const auto &color : glassColors) {
    sf::Texture tex;
    std::string path = "assets/glass/" + color + "_stained_glass.png";
    if (!tex.loadFromFile(path)) {
      throw AssetLoadException(path, "Glass Texture");
    }
    glassTextures.push_back(tex);
  }

  // Randomize default glass color
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::vector<int> validIndices;
  for (int i = 0; i < 16; ++i) {
    if (i != 7 && i != 4) // Exclude Light Gray (7) and Gray (4) as they are hard to see
      validIndices.push_back(i);
    // Note: These colors are still loaded and valid for DiscoFever mode 
  }
  std::uniform_int_distribution<> dis(0, static_cast<int>(validIndices.size()) - 1);
  defaultGlassColorIndex = validIndices[dis(gen)];
  
  // Load effect display assets
  try {
    effectDisplay.loadAssets();
  } catch (const std::exception& e) {
    std::cerr << "Error loading effect assets: " << e.what() << std::endl;
  }
}

void GridRenderer::setDiscoFeverMode(bool enabled) {
  isDiscoFeverMode = enabled;
}

void GridRenderer::drawGameInfo(sf::RenderWindow &window) const {
  // Update HUD animations
  float dt = animationClock.restart().asSeconds();
  
  // Helper to find AlchemyMode in decorator chain
  auto findAlchemyMode = [](GameMode* mode) -> AlchemyMode* {
    if (!mode) return nullptr;
    
    // Try direct cast
    if (auto* alchemy = dynamic_cast<AlchemyMode*>(mode)) {
      return alchemy;
    }
    
    // Traverse decorator chain
    const GameMode* current = mode;
    while (current) {
      if (const auto* decorator = dynamic_cast<const GameModeDecorator*>(current)) {
        if (auto* alchemy = dynamic_cast<AlchemyMode*>(decorator->getWrappedMode())) {
          return alchemy;
        }
        current = decorator->getWrappedMode();
      } else {
        break;
      }
    }
    return nullptr;
  };
  
  // Check for Hunger and Saturation effects from AlchemyMode
  bool hasHunger = false;
  bool hasSaturation = false;
  
  if (const auto* alchemyMode = findAlchemyMode(grid.getMode())) {
    hasHunger = alchemyMode->hasEffect(EffectType::Hunger);
    hasSaturation = alchemyMode->hasEffect(EffectType::Saturation);
  }
  
  int currentMistakes = grid.get_mistakes();

  if (const_cast<MinecraftHUD&>(minecraftHUD).update(dt, hasHunger, hasSaturation)) {
      const_cast<Grid&>(grid).damagePlayer();
  }
  
  if (currentMistakes != lastMistakes) {
    // Mistakes changed (damage or healing) - trigger flash
    heartDisplay.triggerFlash();
  }
  lastMistakes = currentMistakes;
  int maxMistakes = grid.get_max_mistakes();
  int score = grid.get_score();

  // Show poisoned hearts if Poison effect is active
  // Show withered hearts if Time Mode is active (and not poisoned)
  bool isPoisoned = false;
  if (const auto* alchemyMode = findAlchemyMode(grid.getMode())) {
    isPoisoned = alchemyMode->hasEffect(EffectType::Poison);
  }
  
  bool isWithered = grid.is_time_mode();
  
  bool showStats = grid.shouldShowSurvivalStats();

  sf::View originalView = window.getView();
  window.setView(window.getDefaultView());
  
  auto& hud = const_cast<MinecraftHUD&>(minecraftHUD);
  hud.setShowHearts(showStats);
  hud.setShowHunger(showStats);
  
  // Set max health from game mode's maxMistakes
  hud.setMaxHealth(maxMistakes);
  
  hud.draw(window, score, currentMistakes, maxMistakes, isPoisoned, hasHunger, isWithered);
  
  window.setView(originalView);
}

void GridRenderer::drawHintTabs(sf::RenderWindow &window) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = static_cast<float>(maxRowWidth) * cellSize * 0.8f;
  float colHintsHeight = static_cast<float>(maxColHeight) * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  auto winSize = window.getSize();
  float scaleX = static_cast<float>(winSize.x) / 1280.0f;
  float scaleY = static_cast<float>(winSize.y) / 720.0f;
  float uiScale = std::min(scaleX, scaleY);

  int n = grid.get_size();
  float padding = 4.0f * uiScale;

  float tabWidth = cellSize - padding;


  sf::Sprite capSprite(hintTabTexture);
  sf::Sprite bodySprite(hintTabTexture);

  int texW = static_cast<int>(hintTabTexture.getSize().x);
  int texH = static_cast<int>(hintTabTexture.getSize().y);
  int capH = 6;
  int bodyH = texH - capH;

  capSprite.setTextureRect(sf::IntRect({0, 0}, {texW, capH}));
  bodySprite.setTextureRect(sf::IntRect({0, capH}, {texW, bodyH}));

  float spriteScaleX = tabWidth / static_cast<float>(texW);
  float slotHeight = cellSize * 0.8f;
  float spriteScaleY_Body = slotHeight / static_cast<float>(bodyH);
  float spriteScaleY_Cap = spriteScaleX;

  bodySprite.setScale({spriteScaleX, spriteScaleY_Body});
  capSprite.setScale({spriteScaleX, spriteScaleY_Cap});

  const auto &colHints = hints.get_col_hints();
  for (int j = 0; j < n; ++j) {
    float x = gridOffset.x + static_cast<float>(j) * cellSize + padding / 2.0f;

    float bottomY = gridOffset.y - 8.0f * uiScale;

    size_t currentColHeight = colHints[j].size();
    if (currentColHeight == 0)
      currentColHeight = 1;

    for (size_t k = 0; k < currentColHeight; ++k) {
      float y = bottomY - static_cast<float>(k + 1) * slotHeight;
      bodySprite.setPosition({x, y});
      window.draw(bodySprite);
    }

    float capY = bottomY - static_cast<float>(currentColHeight) * slotHeight -
                 (static_cast<float>(capH) * spriteScaleY_Cap);

    capSprite.setPosition({x, capY});
    window.draw(capSprite);
  }


  bodySprite.setRotation(sf::degrees(-90.f));
  capSprite.setRotation(sf::degrees(-90.f));

  bodySprite.setOrigin({0.f, 0.f});
  capSprite.setOrigin({0.f, 0.f});

  const auto &rowHints = hints.get_row_hints();
  for (int i = 0; i < n; ++i) {
    float y = gridOffset.y + static_cast<float>(i) * cellSize + padding / 2.0f;
    float rightX = gridOffset.x - 14.0f * uiScale;

    size_t currentRowWidth = rowHints[i].size();
    if (currentRowWidth == 0)
      currentRowWidth = 1;

    for (size_t k = 0; k < currentRowWidth; ++k) {
      float visualBodyWidth = bodySprite.getGlobalBounds().size.x;
      float visualBodyHeight = bodySprite.getGlobalBounds().size.y;

      float slotX = rightX - static_cast<float>(k + 1) * visualBodyWidth;
      float slotY = y + visualBodyHeight;

      bodySprite.setPosition({slotX, slotY});
      window.draw(bodySprite);
    }

    float visualCapWidth = capSprite.getGlobalBounds().size.x;
    float visualCapHeight = capSprite.getGlobalBounds().size.y;

    float visualBodyWidth = bodySprite.getGlobalBounds().size.x;
    float capX = rightX -
                 static_cast<float>(currentRowWidth) * visualBodyWidth -
                 visualCapWidth;
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
  // Helper to find AlchemyMode in decorator chain  
  auto findAlchemyMode = [](GameMode* mode) -> AlchemyMode* {
    if (!mode) return nullptr;
    if (auto* alchemy = dynamic_cast<AlchemyMode*>(mode)) return alchemy;
    const GameMode* current = mode;
    while (current) {
      if (const auto* decorator = dynamic_cast<const GameModeDecorator*>(current)) {
        if (auto* alchemy = dynamic_cast<AlchemyMode*>(decorator->getWrappedMode())) return alchemy;
        current = decorator->getWrappedMode();
      } else break;
    }
    return nullptr;
  };
  
  if (isDiscoFeverMode) {
    float deltaTime;
    // We use a static clock to track time between frames
    // This is a bit of a hack since draw is const and we don't have deltaTime passed in
    static sf::Clock discoClock;
    static float lastTime = 0.0f;
    float currentTime = discoClock.getElapsedTime().asSeconds();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Avoid huge delta times (e.g. first frame or after pause)
    if (deltaTime > 0.1f)
      deltaTime = 0.016f;

    colorTimer += deltaTime;
    if (colorTimer >= 0.1f) { // Change color every 0.1 seconds
      colorTimer = 0.0f;
      currentColorOffset++;
    }
  }

  const auto &hints = grid.get_hints();
  int n = grid.get_size();

  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = static_cast<float>(maxRowWidth) * cellSize * 0.8f;
  float colHintsHeight = static_cast<float>(maxColHeight) * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  drawHintTabs(window);

  // Draw Background
  float gridWidth = static_cast<float>(n) * cellSize;
  float gridHeight = gridWidth;
  float padding = 16.0f;

  auto winSize = window.getSize();
  float scaleX = static_cast<float>(winSize.x) / 1280.0f;
  float scaleY = static_cast<float>(winSize.y) / 720.0f;
  float uiScale = std::min(scaleX, scaleY);



  float patchScale = std::round(uiScale);
  if (patchScale < 1.f) patchScale = 1.f;

  auto &mutablePatch = const_cast<NinePatch &>(backgroundPatch);
  mutablePatch.setPixelSnapping(true);
  mutablePatch.setPatchScale(patchScale);



  float scaledPadding = std::round(padding * uiScale);

  float bgWidth  = std::round(gridWidth + scaledPadding * 2);
  float bgHeight = std::round(gridHeight + scaledPadding * 2);
  
  mutablePatch.setSize(bgWidth, bgHeight);
  
  sf::Vector2f bgPos = {
    std::round(gridOffset.x - scaledPadding),
    std::round(gridOffset.y - scaledPadding)
  };
  mutablePatch.setPosition(bgPos);
  window.draw(backgroundPatch);
  
  // Draw effects to the right of the grid
  // Get effects from AlchemyMode if active
  
  std::vector<ActiveEffect> activeEffects;
  if (const auto* alchemyMode = findAlchemyMode(grid.getMode())) {
    activeEffects = alchemyMode->getActiveEffects();
  }
  
  if (!activeEffects.empty()) {
    float effectPadding = 10.0f * uiScale;
    sf::Vector2f effectPos;
    effectPos.x = bgPos.x + bgWidth + effectPadding;
    effectPos.y = bgPos.y;
    
    effectDisplay.draw(window, uiScale, activeEffects, effectPos);
  }

  if (true) {
    const auto &rowHints = hints.get_row_hints();
    for (size_t i = 0; i < rowHints.size(); ++i) {
      float rowY =
          gridOffset.y + static_cast<float>(i) * cellSize + cellSize * 0.5f;
      float slotHeight = cellSize * 0.8f;

      for (int j = static_cast<int>(rowHints[i].size()) - 1; j >= 0; --j) {
        int k = static_cast<int>(rowHints[i].size()) - 1 - j;

        float slotCenter = gridOffset.x - (static_cast<float>(k) * slotHeight) -
                           (slotHeight / 2.0f);
        slotCenter -= 16.0f * uiScale;
        unsigned int fontSize = static_cast<unsigned int>(
            std::min(28.f * uiScale, cellSize * 0.5f));

        if (grid.isHintWebbed(true, static_cast<int>(i), j)) {
          sf::Sprite webSprite(webTexture);
          float scale =
              (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
          webSprite.setScale({scale, scale});
          webSprite.setOrigin(
              {static_cast<float>(webTexture.getSize().x) / 2.f,
               static_cast<float>(webTexture.getSize().y) / 2.f});

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

          webSprite.setPosition({textX + bounds.size.x / 2.f,
                                 textY + bounds.size.y / 2.f + 2.0f * uiScale});
          window.draw(webSprite);

          // Draw break texture
          int health =
              grid.get_hints().getWebHealth(true, static_cast<int>(i), j);
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
      float colX =
          gridOffset.x + static_cast<float>(j) * cellSize + cellSize * 0.5f;
      float slotHeight = cellSize * 0.8f;

      for (int i = static_cast<int>(colHints[j].size()) - 1; i >= 0; --i) {
        int k = static_cast<int>(colHints[j].size()) - 1 - i;

        float slotCenterY = gridOffset.y -
                            (static_cast<float>(k) * slotHeight) -
                            (slotHeight / 2.0f);
        slotCenterY -= 16.0f * uiScale;



        unsigned int fontSize = static_cast<unsigned int>(
            std::min(28.f * uiScale, cellSize * 0.5f));

        if (grid.isHintWebbed(false, static_cast<int>(j), i)) {
          sf::Sprite webSprite(webTexture);
          float scale =
              (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
          webSprite.setScale({scale, scale});
          webSprite.setOrigin(
              {static_cast<float>(webTexture.getSize().x) / 2.f,
               static_cast<float>(webTexture.getSize().y) / 2.f});

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

          webSprite.setPosition({textX + bounds.size.x / 2.f,
                                 textY + bounds.size.y / 2.f + 4.0f * uiScale});
          window.draw(webSprite);

          // Draw break texture
          int health =
              grid.get_hints().getWebHealth(false, static_cast<int>(j), i);
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
      sf::Vector2f pos = {gridOffset.x + static_cast<float>(j) * cellSize,
                          gridOffset.y + static_cast<float>(i) * cellSize};

      // Draw slot texture
      sf::Sprite slotSprite(blockTexture);
      float slotScaleX =
          cellSize / static_cast<float>(blockTexture.getSize().x);
      float slotScaleY =
          cellSize / static_cast<float>(blockTexture.getSize().y);
      slotSprite.setScale({slotScaleX, slotScaleY});
      slotSprite.setPosition(pos);
      window.draw(slotSprite);

      if (grid.get_block(i, j).is_completed()) {
        if (!glassTextures.empty()) {
          int textureIndex = defaultGlassColorIndex;
          if (isDiscoFeverMode) {
            size_t h = std::hash<int>{}(i) ^ (std::hash<int>{}(j) << 1);

            textureIndex = static_cast<int>((h + currentColorOffset) % glassTextures.size());
          }

          sf::Sprite glassSprite(glassTextures[textureIndex]);

          float glassScaleFactor = 0.9f;
          float targetSize = cellSize * glassScaleFactor;

          float glassScaleX =
              targetSize /
              static_cast<float>(glassTextures[textureIndex].getSize().x);
          float glassScaleY =
              targetSize /
              static_cast<float>(glassTextures[textureIndex].getSize().y);

          glassSprite.setScale({glassScaleX, glassScaleY});

          glassSprite.setOrigin(
              {static_cast<float>(glassTextures[textureIndex].getSize().x) /
                   2.0f,
               static_cast<float>(glassTextures[textureIndex].getSize().y) /
                   2.0f});
          glassSprite.setPosition(
              {pos.x + cellSize / 2.0f, pos.y + cellSize / 2.0f});

          window.draw(glassSprite);
        } else {
          float blockPadding = cellSize * 0.15f;
          sf::RectangleShape rect(sf::Vector2f(cellSize - 2 * blockPadding,
                                               cellSize - 2 * blockPadding));
          rect.setPosition({pos.x + blockPadding, pos.y + blockPadding});
          rect.setFillColor(sf::Color::Black);
          window.draw(rect);
        }
      }
    }
  }

  grid.drawMode(window);
}

void GridRenderer::handleClick(const sf::Vector2i &mousePos) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = static_cast<float>(maxRowWidth) * cellSize * 0.8f;
  float colHintsHeight = static_cast<float>(maxColHeight) * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  if (static_cast<float>(mousePos.x) < gridOffset.x ||
      static_cast<float>(mousePos.y) < gridOffset.y)
    return;

  int n = grid.get_size();
  int x = static_cast<int>((static_cast<float>(mousePos.y) - gridOffset.y) /
                           cellSize);
  int y = static_cast<int>((static_cast<float>(mousePos.x) - gridOffset.x) /
                           cellSize);

  if (x >= 0 && x < n && y >= 0 && y < n)
    grid.toggle_block(x, y);
}

sf::Vector2f GridRenderer::getHintCenter(bool isRow, int line,
                                         int index) const {
  const auto &hints = grid.get_hints();
  size_t maxRowWidth = hints.get_max_row_width();
  size_t maxColHeight = hints.get_max_col_height();

  float rowHintsWidth = static_cast<float>(maxRowWidth) * cellSize * 0.8f;
  float colHintsHeight = static_cast<float>(maxColHeight) * cellSize * 0.8f;

  sf::Vector2f gridOffset = {offset.x + rowHintsWidth,
                             offset.y + colHintsHeight};

  if (isRow) {
    const auto &rowHints = hints.get_row_hints();
    if (line >= rowHints.size() || index >= rowHints[line].size())
      return {0, 0};

    float rowY =
        gridOffset.y + static_cast<float>(line) * cellSize + cellSize * 0.5f;
    float startX = gridOffset.x - 20.f;

    // Calculate X position based on index (reverse order)
    int reverseIndex = static_cast<int>(rowHints[line].size()) - 1 - index;
    float currentX =
        startX - static_cast<float>(reverseIndex) * (cellSize * 0.8f);

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

    float colX =
        gridOffset.x + static_cast<float>(line) * cellSize + cellSize * 0.5f;
    float startY = gridOffset.y - 10.f;

    int reverseIndex = static_cast<int>(colHints[line].size()) - 1 - index;
    float currentY =
        startY - static_cast<float>(reverseIndex) * (cellSize * 0.8f);

    sf::Text tempText(font, std::to_string(colHints[line][index]));
    tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
    auto bounds = tempText.getLocalBounds();

    float textX = colX - bounds.size.x * 0.5f - bounds.position.x;
    float textY = currentY - bounds.size.y - bounds.position.y;

    return {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f};
  }
}

Grid::WebDamageResult
GridRenderer::handleHintClick(const sf::Vector2i &mousePos) const {
  const auto &hints = grid.get_hints();

  // Check all webbed hints to see if clicked
  const auto &rowHints = hints.get_row_hints();
  for (size_t i = 0; i < rowHints.size(); ++i) {
    for (size_t j = 0; j < rowHints[i].size(); ++j) {
      if (grid.isHintWebbed(true, static_cast<int>(i), static_cast<int>(j))) {
        sf::Vector2f center =
            getHintCenter(true, static_cast<int>(i), static_cast<int>(j));
        float radius = cellSize * 0.4f; // Approximate hit radius

        float dx = static_cast<float>(mousePos.x) - center.x;
        float dy = static_cast<float>(mousePos.y) - center.y;
        if (dx * dx + dy * dy <= radius * radius) {
          return const_cast<Grid &>(grid).damageWeb(
              true, static_cast<int>(i), static_cast<int>(j)); // Use damageWeb
        }
      }
    }
  }

  const auto &colHints = hints.get_col_hints();
  for (size_t j = 0; j < colHints.size(); ++j) {
    for (size_t i = 0; i < colHints[j].size(); ++i) {
      if (grid.isHintWebbed(false, static_cast<int>(j), static_cast<int>(i))) {
        sf::Vector2f center =
            getHintCenter(false, static_cast<int>(j), static_cast<int>(i));
        float radius = cellSize * 0.4f;

        float dx = static_cast<float>(mousePos.x) - center.x;
        float dy = static_cast<float>(mousePos.y) - center.y;
        if (dx * dx + dy * dy <= radius * radius) {
          return const_cast<Grid &>(grid).damageWeb(
              false, static_cast<int>(j), static_cast<int>(i)); // Use damageWeb
        }
      }
    }
  }
  return Grid::WebDamageResult::None;
}


