#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

#include "GameMode.h"
#include "GameConfig.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <vector>

enum class BackgroundType { Desert, Cave, Mineshaft, Classic };

class GameBackground {
public:
  GameBackground();

  void selectBackground(const GameConfig &config);
  void update(float deltaTime, sf::Vector2u windowSize, bool shouldScroll = true);
  void draw(sf::RenderWindow &window) const;

private:
  sf::Texture desertTexture;
  sf::Texture caveTexture;
  sf::Texture mineshaftTexture;
  sf::Texture classicTexture;

  mutable std::optional<sf::Sprite> currentBackground1;
  mutable std::optional<sf::Sprite> currentBackground2;
  BackgroundType currentType;
  
  float offset;
  float speed;
  bool loaded;

  // Audio
  std::vector<sf::SoundBuffer> caveBuffers;
  std::optional<sf::Sound> ambientSound;
  float ambientTimer;

  void loadTextures();
  void loadSounds();
  
public:
  void setVolume(float volume) {
      if (ambientSound) ambientSound->setVolume(volume);
      currentVolume = volume;
  }
private:
  float currentVolume = 100.0f;
};

#endif // GAMEBACKGROUND_H
