#include "SplashText.h"
#include "Exceptions.h"
#include "ShadowedText.h"
#include <fstream>
#include <random>

SplashText::SplashText()
    : font(), text(font), increasing(true), currentScale(1.0f), speed(0.3f) {
  if (!font.openFromFile("assets/Monocraft.ttf")) {
    throw AssetLoadException("assets/Monocraft.ttf", "Font");
  }
  loadMessages();
  pickRandomMessage();
}

void SplashText::loadMessages() {
  std::ifstream file("assets/splash_text.txt");
  if (!file.is_open()) {
    throw AssetLoadException("assets/splash_text.txt", "File");
  }

  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      messages.push_back(line);
    }
  }
  file.close();

  if (messages.empty()) {
    messages.emplace_back("Picross Fun!");
  }
}

void SplashText::pickRandomMessage() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, messages.size() - 1);
  std::string randomMessage = messages[dist(gen)];

  text = sf::Text(font, randomMessage);
  text.setCharacterSize(30);
  text.setFillColor(sf::Color(213, 222, 82)); // Galben Minecraft
}

void SplashText::update(float deltaTime) {

  if (increasing) {
    currentScale += speed * deltaTime;
  } else {
    currentScale -= speed * deltaTime;
  }

  if (currentScale > 1.25f) {
    currentScale = 1.25f;
    increasing = false;
  } else if (currentScale < 1.0f) {
    currentScale = 1.0f;
    increasing = true;
  }
}

void SplashText::draw(sf::RenderWindow &window, const sf::Sprite &titleSprite,
                      float scale, float scaleY, float logoOriginalWidth,
                      float logoOriginalHeight) {


  float logoX = static_cast<float>(window.getSize().x) / 2.0f;
  float logoY = 100.0f * scaleY;
  float logoScale = scale * 0.5f;

  float logoWidth = logoOriginalWidth * logoScale;
  float logoHeight = logoOriginalHeight * logoScale;

  float splashX = logoX + logoWidth * 0.42f;
  float splashY = logoY + logoHeight * 0.05f;

  float finalScale = scale * currentScale;
  text.setCharacterSize(static_cast<unsigned int>(30.0f * finalScale));

  text.setRotation(sf::degrees(-20.0f));

  auto bounds = text.getLocalBounds();
  text.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

  text.setPosition({splashX, splashY});
  ShadowedText::draw(window, text, scale);
}
