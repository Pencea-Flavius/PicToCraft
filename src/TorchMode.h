#ifndef OOP_TORCHMODE_H
#define OOP_TORCHMODE_H

#include "GameMode.h"
#include "ParticleSystem.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

class TorchMode : public GameModeDecorator {
public:
  explicit TorchMode(std::unique_ptr<GameMode> mode);
  TorchMode(const TorchMode &other) = delete;
  ~TorchMode() override = default;

  void draw(sf::RenderWindow &window) const override;
  void update(float deltaTime) override;
  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  void print(std::ostream &os) const override {
    os << "TorchMode + ";
    GameModeDecorator::print(os);
  }
  [[nodiscard]] std::string getName() const override { return "Torch Mode"; }

private:
  mutable sf::RenderTexture lightLayer;
  mutable std::optional<sf::Sprite> lightSprite;
  mutable sf::Texture lightTexture;
  mutable bool lightTextureCreated = false;

  std::vector<sf::SoundBuffer> fireBuffers;
  sf::SoundBuffer dummyBuffer;
  sf::Sound fireSound;

  float silenceTimer;
  bool inSilence;

  void createLightTexture() const;
  void playNextFireSound();

  ParticleSystem particleSystem;
};

#endif // OOP_TORCHMODE_H