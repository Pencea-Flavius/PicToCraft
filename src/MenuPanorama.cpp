#include "MenuPanorama.h"
#include <iostream>

MenuPanorama::MenuPanorama() : loaded(false), offset(0.0f), speed(30.0f) {
  if (panoramaTexture.loadFromFile("assets/panorama.jpg")) {
    panoramaTexture.setRepeated(true);
    panoramaSprite1 = sf::Sprite(panoramaTexture);
    panoramaSprite2 = sf::Sprite(panoramaTexture);
    loaded = true;
    std::cout << "Panorama incarcata: " << panoramaTexture.getSize().x << "x"
              << panoramaTexture.getSize().y << "\n";
  } else {
    std::cerr << "Eroare la incarcarea panoramei!\n";
  }
}

void MenuPanorama::update(float deltaTime) {
  if (!loaded)
    return;

  offset += speed * deltaTime;

  float textureWidth = static_cast<float>(panoramaTexture.getSize().x);
  if (offset >= textureWidth) {
    offset -= textureWidth;
  }
}

void MenuPanorama::draw(sf::RenderWindow &window) {
  if (!loaded || !panoramaSprite1 || !panoramaSprite2)
    return;

  auto winSize = window.getSize();

  float scaleY = static_cast<float>(winSize.y) /
                 static_cast<float>(panoramaTexture.getSize().y);
  float textureWidth = static_cast<float>(panoramaTexture.getSize().x) * scaleY;

  panoramaSprite1->setScale({scaleY, scaleY});
  panoramaSprite1->setPosition({-offset * scaleY, 0.0f});

  panoramaSprite2->setScale({scaleY, scaleY});
  panoramaSprite2->setPosition({textureWidth - offset * scaleY, 0.0f});

  window.draw(*panoramaSprite1);
  window.draw(*panoramaSprite2);
}
