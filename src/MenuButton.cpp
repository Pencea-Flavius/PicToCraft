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

sf::Texture MenuButton::textFieldTexture;
sf::Texture MenuButton::textFieldHighlightedTexture;
bool MenuButton::textFieldTexturesLoaded = false;

MenuButton::MenuButton(const std::string &label, const sf::Font &font,
                       const sf::Texture &texture, unsigned int fontSize)
    : currentTexture(&texture), m_ninePatch(texture, 4, 0),
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
      throw AssetLoadException("assets/buttons/slider_handle.png", "Texture");
    }
    if (!sliderHandleHighlightedTexture.loadFromFile(
            "assets/buttons/slider_handle_highlighted.png")) {
      throw AssetLoadException(
          "assets/buttons/slider_handle_highlighted.png", "Texture");
    }
    sliderTexturesLoaded = true;
  }

  if (!textFieldTexturesLoaded) {
      if (!textFieldTexture.loadFromFile("assets/buttons/text_field.png")) {
          throw AssetLoadException("assets/buttons/text_field.png", "Texture");
      }
      if (!textFieldHighlightedTexture.loadFromFile("assets/buttons/text_field_highlighted.png")) {
          throw AssetLoadException("assets/buttons/text_field_highlighted.png", "Texture");
      }
      textFieldTexturesLoaded = true;
  }

  // setupNinePatch(); // Removed
  m_ninePatch.setPixelSnapping(true); // Ensure pixel-perfect rendering
  text.setFillColor(sf::Color::White);
}

MenuButton::~MenuButton() = default;

void MenuButton::update(float scale, float x, float y, float widthScale,
                        float heightScale, const sf::Vector2f &mousePos) {
  currentScale = scale;
  position = {std::round(x), std::round(y)};

  float baseWidth = 400.0f;
  float baseHeight = 40.0f;

  // Round sizes to integers to avoid sub-pixel artifacts
  buttonSize.x = std::round(baseWidth * scale * widthScale);
  buttonSize.y = std::round(baseHeight * scale * heightScale);

  // Update NinePatch
  m_ninePatch.setSize(buttonSize.x, buttonSize.y);
  m_ninePatch.setPosition(
      {position.x - buttonSize.x / 2.0f, position.y - buttonSize.y / 2.0f});

  float borderScale = 2.0f;
  float visualBorderScale = std::round(scale * borderScale);
  if (visualBorderScale < 1.0f)
    visualBorderScale = 1.0f;

  m_ninePatch.setPatchScale(visualBorderScale);

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
    float stepSize = 1.0f / static_cast<float>(sliderSteps - 1);
    int step = static_cast<int>(std::round(rawValue / stepSize));
    sliderValue = static_cast<float>(step) * stepSize;
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
  m_ninePatch.setTexture(texture);
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

    m_ninePatch.setColor(spriteColor);
    window.draw(m_ninePatch);

    text.setCharacterSize(static_cast<unsigned int>(
        static_cast<float>(baseFontSize) * currentScale));
    auto textBounds = text.getLocalBounds();
    float textX = std::round(position.x - textBounds.size.x / 2.0f -
                             textBounds.position.x);
    float textY = std::round(position.y - textBounds.size.y / 2.0f -
                             textBounds.position.y);
    text.setPosition({textX, textY});
    text.setFillColor(textColor);
    ShadowedText::draw(window, text, currentScale);
  } else if (style == Style::Slider) {
    sf::Color trackColor = sf::Color::White;
    m_ninePatch.setColor(trackColor);
    window.draw(m_ninePatch);

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

    text.setCharacterSize(static_cast<unsigned int>(
        static_cast<float>(baseFontSize) * currentScale));

    auto textBounds = text.getLocalBounds();
    float textX = std::round(position.x - textBounds.size.x / 2.0f -
                             textBounds.position.x);
    float textY = std::round(position.y - textBounds.size.y / 2.0f -
                             textBounds.position.y);

    text.setFillColor(sf::Color::White);
    text.setPosition({textX, textY});

    ShadowedText::draw(window, text, currentScale);
  } else if (style == Style::TextField) {
      const sf::Texture* texToUse = (hovered || selected) ? &textFieldHighlightedTexture : &textFieldTexture;
      if (currentTexture != texToUse) {
          const_cast<MenuButton*>(this)->setTexture(*texToUse);
      }
      
      m_ninePatch.setColor(sf::Color::White);
      window.draw(m_ninePatch);

      text.setCharacterSize(static_cast<unsigned int>(
          static_cast<float>(baseFontSize) * currentScale));

      auto textBounds = text.getLocalBounds();
      // Center text
      float textX = std::round(position.x - textBounds.size.x / 2.0f - textBounds.position.x);
      float textY = std::round(position.y - textBounds.size.y / 2.0f - textBounds.position.y);
      
      text.setFillColor(sf::Color::White);
      text.setPosition({textX, textY});
      
      ShadowedText::draw(window, text, currentScale);
  } else {
    sf::Color spriteColor =
        hovered ? sf::Color(160, 190, 240) : sf::Color::White;

    m_ninePatch.setColor(spriteColor);
    window.draw(m_ninePatch);

    text.setCharacterSize(static_cast<unsigned int>(
        static_cast<float>(baseFontSize) * currentScale));

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