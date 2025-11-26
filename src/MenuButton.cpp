#include "MenuButton.h"
#include "Exceptions.h"
#include "ShadowedText.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

sf::SoundBuffer MenuButton::clickSoundBuffer;
std::unique_ptr<sf::Sound> MenuButton::clickSound = nullptr;
bool MenuButton::soundInitialized = false;

sf::Texture MenuButton::sliderHandleTexture;
sf::Texture MenuButton::sliderHandleHighlightedTexture;
bool MenuButton::sliderTexturesLoaded = false;

MenuButton::MenuButton(const std::string &label, const sf::Font &font,
                       const sf::Texture &texture, unsigned int fontSize)
    : currentTexture(&texture), ninePatchConfig(DEFAULT_NINE_PATCH),
      text(font, label, fontSize), hovered(false), baseFontSize(fontSize),
      currentScale(1.0f), buttonSize(200.0f, 20.0f), position(0.0f, 0.0f) {

  if (!soundInitialized) {
    if (!clickSoundBuffer.loadFromFile("assets/sound/click.mp3")) {
      throw AssetLoadException("assets/sound/click.mp3", "Sound");
    }
    clickSound = std::make_unique<sf::Sound>(clickSoundBuffer);
    clickSound->setVolume(50.0f);
    soundInitialized = true;
  }

  if (!sliderTexturesLoaded) {
    if (!sliderHandleTexture.loadFromFile("assets/buttons/slider_handle.png")) {
      throw std::runtime_error(
          "Failed to load assets/buttons/slider_handle.png");
    }
    if (!sliderHandleHighlightedTexture.loadFromFile(
            "assets/buttons/slider_handle_highlighted.png")) {
      throw std::runtime_error(
          "Failed to load assets/buttons/slider_handle_highlighted.png");
    }
    sliderTexturesLoaded = true;
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
  float visualBorderScale = std::round(scale * borderScale);
  if (visualBorderScale < 1.0f)
    visualBorderScale = 1.0f;

  float scaledCornerSize = cornerSize * visualBorderScale;

  float horizontalScale =
      (targetWidth - 2.0f * scaledCornerSize) / static_cast<float>(edgeWidth);
  float verticalScale =
      (targetHeight - 2.0f * scaledCornerSize) / static_cast<float>(edgeHeight);

  float snappedTopLeftX = std::round(topLeftX);
  float snappedTopLeftY = std::round(topLeftY);

  // 0: Top-Left
  patchSprites[0].setPosition({snappedTopLeftX, snappedTopLeftY});
  patchSprites[0].setScale({visualBorderScale, visualBorderScale});

  // 1: Top
  patchSprites[1].setPosition(
      {snappedTopLeftX + scaledCornerSize, snappedTopLeftY});
  patchSprites[1].setScale({horizontalScale, visualBorderScale});

  // 2: Top-Right
  patchSprites[2].setPosition(
      {snappedTopLeftX + targetWidth - scaledCornerSize, snappedTopLeftY});
  patchSprites[2].setScale({visualBorderScale, visualBorderScale});

  // 3: Left
  patchSprites[3].setPosition(
      {snappedTopLeftX, snappedTopLeftY + scaledCornerSize});
  patchSprites[3].setScale({visualBorderScale, verticalScale});

  // 4: Center
  patchSprites[4].setPosition(
      {snappedTopLeftX + scaledCornerSize, snappedTopLeftY + scaledCornerSize});
  patchSprites[4].setScale({horizontalScale, verticalScale});

  // 5: Right
  patchSprites[5].setPosition({snappedTopLeftX + targetWidth - scaledCornerSize,
                               snappedTopLeftY + scaledCornerSize});
  patchSprites[5].setScale({visualBorderScale, verticalScale});

  // 6: Bottom-Left
  patchSprites[6].setPosition(
      {snappedTopLeftX, snappedTopLeftY + targetHeight - scaledCornerSize});
  patchSprites[6].setScale({visualBorderScale, visualBorderScale});

  // 7: Bottom
  patchSprites[7].setPosition(
      {snappedTopLeftX + scaledCornerSize,
       snappedTopLeftY + targetHeight - scaledCornerSize});
  patchSprites[7].setScale({horizontalScale, visualBorderScale});

  // 8: Bottom-Right
  patchSprites[8].setPosition(
      {snappedTopLeftX + targetWidth - scaledCornerSize,
       snappedTopLeftY + targetHeight - scaledCornerSize});
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

  if (style == Style::Slider && isDragging) {
    handleDrag(mousePos);
  }
}

void MenuButton::setSliderValue(float value) {
  sliderValue = std::clamp(value, 0.0f, 1.0f);
}

float MenuButton::getSliderValue() const { return sliderValue; }

void MenuButton::setSliderSteps(int steps) { sliderSteps = steps; }

void MenuButton::handleDrag(const sf::Vector2f &mousePos) {
  float left = position.x - buttonSize.x / 2.0f;
  float width = buttonSize.x;
  float relativeX = mousePos.x - left;
  float rawValue = std::clamp(relativeX / width, 0.0f, 1.0f);

  if (sliderSteps > 1) {
    float stepSize = 1.0f / (sliderSteps - 1);
    int step = static_cast<int>(std::round(rawValue / stepSize));
    sliderValue = step * stepSize;
  } else {
    sliderValue = rawValue;
  }
}

void MenuButton::stopDrag() { isDragging = false; }

bool MenuButton::isClicked(const sf::Vector2f &mousePos) {
  if (!enabled)
    return false;
  sf::FloatRect bounds(
      {position.x - buttonSize.x / 2.0f, position.y - buttonSize.y / 2.0f},
      {buttonSize.x, buttonSize.y});
  bool clicked = bounds.contains(mousePos);

  if (clicked && style == Style::Slider) {
    isDragging = true;
    handleDrag(mousePos);
  }

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

void MenuButton::setText(const std::string &newText) {
  text.setString(newText);
}

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
    float textX = std::round(position.x - textBounds.size.x / 2.0f -
                             textBounds.position.x);
    float textY = std::round(position.y - textBounds.size.y / 2.0f -
                             textBounds.position.y);
    text.setPosition({textX, textY});
    text.setFillColor(textColor);
    ShadowedText::draw(window, text, currentScale);

    ShadowedText::draw(window, text, currentScale);

  } else if (style == Style::Slider) {
    sf::Color trackColor = sf::Color::White;
    for (auto &sprite : patchSprites) {
      sprite.setColor(trackColor);
      window.draw(sprite);
    }

    sf::Sprite handleSprite(hovered || isDragging
                                ? sliderHandleHighlightedTexture
                                : sliderHandleTexture);

    auto handleBounds = handleSprite.getLocalBounds();
    handleSprite.setOrigin(
        sf::Vector2f(handleBounds.size.x / 2.0f, handleBounds.size.y / 2.0f));

    float handleScale = currentScale * 3;
    handleSprite.setScale({handleScale, handleScale});

    float handleX =
        (position.x - buttonSize.x / 2.0f) + (sliderValue * buttonSize.x);
    handleSprite.setPosition({handleX, position.y});

    window.draw(handleSprite);

    text.setCharacterSize(
        static_cast<unsigned int>(baseFontSize * currentScale));

    auto textBounds = text.getLocalBounds();
    float textX = std::round(position.x - textBounds.size.x / 2.0f -
                             textBounds.position.x);
    float textY = std::round(position.y - textBounds.size.y / 2.0f -
                             textBounds.position.y);

    text.setFillColor(sf::Color::White);
    text.setPosition({textX, textY});

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
    float textX = std::round(position.x - textBounds.size.x / 2.0f -
                             textBounds.position.x);
    float textY = std::round(position.y - textBounds.size.y / 2.0f -
                             textBounds.position.y);

    sf::Color textColor =
        hovered ? sf::Color(255, 255, 160) : sf::Color(221, 221, 221);
    text.setFillColor(textColor);
    text.setPosition({textX, textY});

    ShadowedText::draw(window, text, currentScale);
  }
}
