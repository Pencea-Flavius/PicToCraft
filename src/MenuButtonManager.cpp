#include "MenuButtonManager.h"

MenuButtonManager::MenuButtonManager(const sf::Font &font,
                                     const sf::Texture &buttonTexture,
                                     const sf::Texture &buttonDisabledTexture)
    : font(font), buttonTexture(buttonTexture),
      buttonDisabledTexture(buttonDisabledTexture) {}

void MenuButtonManager::createButtons(const std::vector<std::string> &labels,
                                      unsigned int fontSize) {
  buttons.clear();
  for (const auto &label : labels) {
    buttons.push_back(
        std::make_unique<MenuButton>(label, font, buttonTexture, fontSize));
  }
}

void MenuButtonManager::addSlider(const std::string &label, float initialValue,
                                  int steps, unsigned int fontSize) {
  auto slider = std::make_unique<MenuButton>(label, font, buttonDisabledTexture,
                                             fontSize);
  slider->setStyle(MenuButton::Style::Slider);
  slider->setSliderValue(initialValue);
  slider->setSliderSteps(steps);
  buttons.push_back(std::move(slider));
}

void MenuButtonManager::addButton(const std::string &label,
                                  unsigned int fontSize) {
  buttons.push_back(
      std::make_unique<MenuButton>(label, font, buttonTexture, fontSize));
}

void MenuButtonManager::setButtonText(int index,
                                      const std::string &text) const {
  if (index >= 0 && index < static_cast<int>(buttons.size())) {
    buttons[index]->setText(text);
  }
}

void MenuButtonManager::setButtonEnabled(int index, bool enabled) const {
  if (index >= 0 && index < static_cast<int>(buttons.size())) {
    buttons[index]->setEnabled(enabled);
  }
}

void MenuButtonManager::layoutMainMenu(const sf::RenderWindow &window,
                                       float scale, float scaleY) const {
  if (buttons.size() < 3)
    return;

  float centerX = static_cast<float>(window.getSize().x) / 2.0f;
  float startY = 280.0f * scaleY;
  float spacing = 60.0f * scaleY;

  sf::Vector2f mousePos =
      window.mapPixelToCoords(sf::Mouse::getPosition(window));

  buttons[0]->update(scale, centerX, startY, 1.0f, 1.0f, mousePos);

  float row2Y = startY + spacing;
  float buttonWidth = 400.0f * scale;
  float gap = 4.0f * scale;

  float optX = centerX - buttonWidth * 0.25f - gap / 2;
  float quitX = centerX + buttonWidth * 0.25f + gap / 2;

  buttons[1]->update(scale, optX, row2Y, 0.5f, 1.0f, mousePos);
  buttons[2]->update(scale, quitX, row2Y, 0.5f, 1.0f, mousePos);
}

void MenuButtonManager::layoutGameSetup(
    const sf::RenderWindow &window, float scale, float scaleY, int selectedTab,
    bool isTimeModeAvailable, const sf::Texture &activeButtonTexture,
    const sf::Texture &inactiveButtonTexture) const {

  if (buttons.size() < 2)
    return;

  float centerX = static_cast<float>(window.getSize().x) / 2.0f;
  float topY = 140.0f * scaleY;
  float spacing = 60.0f * scaleY;

  sf::Vector2f mousePos =
      window.mapPixelToCoords(sf::Mouse::getPosition(window));

  float tabSpacing = 210.0f * scale;

  buttons[0]->setStyle(MenuButton::Style::Tab);
  buttons[1]->setStyle(MenuButton::Style::Tab);

  buttons[0]->setSelected(selectedTab == 0);
  buttons[1]->setSelected(selectedTab == 1);

  buttons[0]->setTexture(selectedTab == 0 ? inactiveButtonTexture
                                          : activeButtonTexture);
  buttons[1]->setTexture(selectedTab == 1 ? inactiveButtonTexture
                                          : activeButtonTexture);

  buttons[0]->update(scale, centerX - tabSpacing / 2, topY, 0.5f, 1.0f,
                     mousePos);
  buttons[1]->update(scale, centerX + tabSpacing / 2, topY, 0.5f, 1.0f,
                     mousePos);

  float contentY = topY + spacing * 1.5f;
  for (size_t i = 2; i < buttons.size() - 2; i++) {
    buttons[i]->setStyle(MenuButton::Style::Default);
    buttons[i]->setEnabled(true);

    if (bool shouldDisable =
            (selectedTab == 1 && i == 2 && !isTimeModeAvailable);
        shouldDisable) {
      buttons[i]->setTexture(inactiveButtonTexture);
      buttons[i]->setEnabled(false);
    } else {
      buttons[i]->setTexture(activeButtonTexture);
    }

    buttons[i]->update(scale, centerX,
                       contentY + static_cast<float>(i - 2) * spacing, 1.2f,
                       1.0f, mousePos);
  }

  size_t playIndex = buttons.size() - 2;
  size_t cancelIndex = buttons.size() - 1;

  float footerY = static_cast<float>(window.getSize().y) - 50.0f * scaleY;

  buttons[playIndex]->setStyle(MenuButton::Style::Default);
  buttons[playIndex]->update(scale, centerX - 170.0f * scale, footerY, 0.8f,
                             1.0f, mousePos);

  buttons[cancelIndex]->setStyle(MenuButton::Style::Default);
  buttons[cancelIndex]->update(scale, centerX + 170.0f * scale, footerY, 0.8f,
                               1.0f, mousePos);
}

void MenuButtonManager::layoutOptions(const sf::RenderWindow &window,
                                      float scale, float scaleY) const {
  if (buttons.size() < 3)
    return;

  float centerX = static_cast<float>(window.getSize().x) / 2.0f;
  float startY = 200.0f * scaleY;
  float spacing = 60.0f * scaleY;

  sf::Vector2f mousePos =
      window.mapPixelToCoords(sf::Mouse::getPosition(window));

  buttons[0]->update(scale, centerX, startY, 1.5f, 1.5f, mousePos);

  buttons[1]->update(scale, centerX, startY + spacing * 1.5f, 1.5f, 1.5f,
                     mousePos);
  float footerY = static_cast<float>(window.getSize().y) - 50.0f * scaleY;
  buttons[2]->update(scale, centerX, footerY, 1.5f, 1.5f, mousePos);
}

void MenuButtonManager::handleDrag(const sf::Vector2f &mousePos) const {
  for (auto &button : buttons) {
    if (button->isDraggingSlider()) {
      button->handleDrag(mousePos);
    }
  }
}

void MenuButtonManager::stopDrag() const {
  for (auto &button : buttons) {
    button->stopDrag();
  }
}

float MenuButtonManager::getSliderValue(int index) const {
  if (index >= 0 && index < static_cast<int>(buttons.size())) {
    return buttons[index]->getSliderValue();
  }
  return 0.0f;
}

int MenuButtonManager::handleClick(const sf::Vector2f &mousePos) const {
  for (size_t i = 0; i < buttons.size(); i++) {
    if (buttons[i]->isClicked(mousePos)) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void MenuButtonManager::update(const sf::Vector2f &mousePos) const {
  for (auto &button : buttons) {
    button->update(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, mousePos);
  }
}

void MenuButtonManager::draw(sf::RenderWindow &window) const {
  for (auto &button : buttons) {
    button->draw(window);
  }
}
