#include "MenuButton.h"
#include "ShadowedText.h"
#include <iostream>

sf::SoundBuffer MenuButton::clickSoundBuffer;
std::unique_ptr<sf::Sound> MenuButton::clickSound = nullptr;
bool MenuButton::soundInitialized = false;

MenuButton::MenuButton(const std::string &label, const sf::Font &font,
                       const sf::Texture &texture, unsigned int fontSize)
    : currentTexture(&texture), ninePatchConfig(DEFAULT_NINE_PATCH),
      text(font, label, fontSize), hovered(false), baseFontSize(fontSize),
      currentScale(1.0f), buttonSize(200.0f, 20.0f), position(0.0f, 0.0f) {

  if (!soundInitialized) {
    if (clickSoundBuffer.loadFromFile("assets/sound/click.mp3")) {
      clickSound = std::make_unique<sf::Sound>(clickSoundBuffer);
      clickSound->setVolume(50.0f);
      soundInitialized = true;
    } else {
      std::cerr << "Failed to load click sound" << std::endl;
    }
  }

  setupNinePatch();
  text.setFillColor(sf::Color::White);
}

MenuButton::~MenuButton() = default;

void MenuButton::setupNinePatch() {
  if (!currentTexture)
    return;

  patchSprites.clear();
  patchSprites.reserve(9);

  int cornerSize = ninePatchConfig.cornerSize;

  auto texSize = currentTexture->getSize();
  int texWidth = static_cast<int>(texSize.x);
  int texHeight = static_cast<int>(texSize.y);

  int edgeWidth = texWidth - 2 * cornerSize;
  int edgeHeight = texHeight - 2 * cornerSize;

  std::array<sf::IntRect, 9> rects = {{
      sf::IntRect({0, 0}, {cornerSize, cornerSize}),
      sf::IntRect({cornerSize, 0}, {edgeWidth, cornerSize}),
      sf::IntRect({texWidth - cornerSize, 0}, {cornerSize, cornerSize}),
      sf::IntRect({0, cornerSize}, {cornerSize, edgeHeight}),
      sf::IntRect({cornerSize, cornerSize}, {edgeWidth, edgeHeight}),
      sf::IntRect({texWidth - cornerSize, cornerSize},
                  {cornerSize, edgeHeight}),
      sf::IntRect({0, texHeight - cornerSize}, {cornerSize, cornerSize}),
      sf::IntRect({cornerSize, texHeight - cornerSize},
                  {edgeWidth, cornerSize}),
      sf::IntRect({texWidth - cornerSize, texHeight - cornerSize},
                  {cornerSize, cornerSize}),
  }};

  for (int i = 0; i < 9; i++) {
    patchSprites.emplace_back(*currentTexture);
    patchSprites[i].setTextureRect(rects[i]);
  }
}

void MenuButton::updateNinePatchLayout() {
  int cornerSize = ninePatchConfig.cornerSize;
  int edgeThickness = ninePatchConfig.edgeThickness;

  float targetWidth = buttonSize.x;
  float targetHeight = buttonSize.y;

  float topLeftX = position.x - targetWidth / 2.0f;
  float topLeftY = position.y - targetHeight / 2.0f;

  auto texSize = currentTexture->getSize();
  int texWidth = static_cast<int>(texSize.x);
  int texHeight = static_cast<int>(texSize.y);

  int edgeWidth = texWidth - 2 * cornerSize;
  int edgeHeight = texHeight - 2 * cornerSize;

  float scale = currentScale;
  float borderScale = 1.0f;
  if (cornerSize > 0) {
    borderScale = static_cast<float>(edgeThickness) / cornerSize;
  }
  float visualBorderScale = scale * borderScale;
  float scaledCornerSize = cornerSize * visualBorderScale;

  float horizontalScale =
      (targetWidth - 2.0f * scaledCornerSize) / static_cast<float>(edgeWidth);
  float verticalScale =
      (targetHeight - 2.0f * scaledCornerSize) / static_cast<float>(edgeHeight);

  // 0: Top-Left
  patchSprites[0].setPosition({topLeftX, topLeftY});
  patchSprites[0].setScale({visualBorderScale, visualBorderScale});

  // 1: Top
  patchSprites[1].setPosition({topLeftX + scaledCornerSize, topLeftY});
  patchSprites[1].setScale({horizontalScale, visualBorderScale});

  // 2: Top-Right
  patchSprites[2].setPosition(
      {topLeftX + targetWidth - scaledCornerSize, topLeftY});
  patchSprites[2].setScale({visualBorderScale, visualBorderScale});

  // 3: Left
  patchSprites[3].setPosition({topLeftX, topLeftY + scaledCornerSize});
  patchSprites[3].setScale({visualBorderScale, verticalScale});

  // 4: Center
  patchSprites[4].setPosition(
      {topLeftX + scaledCornerSize, topLeftY + scaledCornerSize});
  patchSprites[4].setScale({horizontalScale, verticalScale});

  // 5: Right
  patchSprites[5].setPosition(
      {topLeftX + targetWidth - scaledCornerSize, topLeftY + scaledCornerSize});
  patchSprites[5].setScale({visualBorderScale, verticalScale});

  // 6: Bottom-Left
  patchSprites[6].setPosition(
      {topLeftX, topLeftY + targetHeight - scaledCornerSize});
  patchSprites[6].setScale({visualBorderScale, visualBorderScale});

  // 7: Bottom
  patchSprites[7].setPosition({topLeftX + scaledCornerSize,
                               topLeftY + targetHeight - scaledCornerSize});
  patchSprites[7].setScale({horizontalScale, visualBorderScale});

  // 8: Bottom-Right
  patchSprites[8].setPosition({topLeftX + targetWidth - scaledCornerSize,
                               topLeftY + targetHeight - scaledCornerSize});
  patchSprites[8].setScale({visualBorderScale, visualBorderScale});
}

void MenuButton::update(float scale, float x, float y, float widthScale,
                        float heightScale, const sf::Vector2f &mousePos) {
  currentScale = scale;
  position = {x, y};

  float baseWidth = 400.0f;
  float baseHeight = 40.0f;

  buttonSize.x = baseWidth * scale * widthScale;
  buttonSize.y = baseHeight * scale * heightScale;

  updateNinePatchLayout();

  sf::FloatRect bounds(
      {position.x - buttonSize.x / 2.0f, position.y - buttonSize.y / 2.0f},
      {buttonSize.x, buttonSize.y});
  hovered = bounds.contains(mousePos);
}

bool MenuButton::isClicked(const sf::Vector2f &mousePos) const {
  if (!enabled)
    return false;
  sf::FloatRect bounds(
      {position.x - buttonSize.x / 2.0f, position.y - buttonSize.y / 2.0f},
      {buttonSize.x, buttonSize.y});
  bool clicked = bounds.contains(mousePos);
  if (clicked && soundInitialized && clickSound) {
    clickSound->play();
  }
  return clicked;
}

void MenuButton::setStyle(Style newStyle) { style = newStyle; }

void MenuButton::setSelected(bool isSelected) { selected = isSelected; }

void MenuButton::setTexture(const sf::Texture &texture) {
  currentTexture = &texture;
  setupNinePatch();
}

void MenuButton::setEnabled(bool isEnabled) { enabled = isEnabled; }

void MenuButton::draw(sf::RenderWindow &window) {
  if (style == Style::Tab) {
    // Only show hover effect if not selected
    sf::Color spriteColor =
        (!selected && hovered) ? sf::Color(160, 190, 240) : sf::Color::White;
    sf::Color textColor =
        (!selected && hovered) ? sf::Color(255, 255, 160) : sf::Color::White;

    for (auto &sprite : patchSprites) {
      sprite.setColor(spriteColor);
      window.draw(sprite);
    }

    text.setCharacterSize(
        static_cast<unsigned int>(baseFontSize * currentScale));
    auto textBounds = text.getLocalBounds();
    float textX = position.x - textBounds.size.x / 2.0f - textBounds.position.x;
    float textY = position.y - textBounds.size.y / 2.0f - textBounds.position.y;
    text.setPosition({textX, textY});
    text.setFillColor(textColor);
    ShadowedText::draw(window, text, currentScale);

  } else {
    sf::Color spriteColor =
        hovered ? sf::Color(160, 190, 240) : sf::Color::White;

    for (auto &sprite : patchSprites) {
      sprite.setColor(spriteColor);
      window.draw(sprite);
    }

    text.setCharacterSize(
        static_cast<unsigned int>(baseFontSize * currentScale));

    auto textBounds = text.getLocalBounds();
    float textX = position.x - textBounds.size.x / 2.0f - textBounds.position.x;
    float textY = position.y - textBounds.size.y / 2.0f - textBounds.position.y;

    sf::Color textColor =
        hovered ? sf::Color(255, 255, 160) : sf::Color(221, 221, 221);
    text.setFillColor(textColor);
    text.setPosition({textX, textY});

    ShadowedText::draw(window, text, currentScale);
  }
}
