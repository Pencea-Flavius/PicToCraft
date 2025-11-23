#ifndef OOP_TIMEMODE_H
#define OOP_TIMEMODE_H

#include "GameMode.h"

class TimeMode : public GameMode {
  int maxHearts;
  float decayTimer;
  const float DECAY_INTERVAL = 10.0f;

public:
  explicit TimeMode(int gridSize);
  ~TimeMode() override = default;

  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override;
  void update(float deltaTime) override;

  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] int getMaxMistakes() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
  [[nodiscard]] bool isTimeMode() const override;
};

#endif // OOP_TIMEMODE_H
