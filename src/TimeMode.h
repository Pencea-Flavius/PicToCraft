#ifndef OOP_TIMEMODE_H
#define OOP_TIMEMODE_H

#include "GameMode.h"
#include <SFML/Audio.hpp>

class TimeMode : public GameModeDecorator {
  float decayTimer;
  float timeLeft;
  float totalTime;
  const float DECAY_INTERVAL = 10.0f;
  int maxHearts;

  sf::SoundBuffer hurtBuffer;
  sf::Sound hurtSound;

public:
  explicit TimeMode(std::unique_ptr<GameMode> mode, int gridSize);
  ~TimeMode() override = default;

  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override;
  void update(float deltaTime) override;

  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] int getMaxMistakes() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;

  [[nodiscard]] int getMistakes() const override;
  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  void print(std::ostream &os) const override {
    os << "TimeMode + ";
    GameModeDecorator::print(os);
  }
  [[nodiscard]] std::string getName() const override { return "Time Mode"; }
};

#endif // OOP_TIMEMODE_H
