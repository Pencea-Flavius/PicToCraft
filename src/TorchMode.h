#ifndef OOP_TORCHMODE_H
#define OOP_TORCHMODE_H

#include "GameMode.h"
#include <SFML/Graphics.hpp>
#include <optional>

class TorchMode : public GameModeDecorator {
public:
  explicit TorchMode(std::unique_ptr<GameMode> mode);
  TorchMode(const TorchMode &other) = delete;
  ~TorchMode() override = default;

  [[nodiscard]] bool isTorchMode() const override;
  void draw(sf::RenderWindow &window) const override;
  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;

private:
  mutable sf::RenderTexture lightLayer;
  mutable std::optional<sf::Sprite> lightSprite;
  mutable sf::Texture lightTexture;
  mutable bool lightTextureCreated = false;

  void createLightTexture() const;
};

#endif // OOP_TORCHMODE_H