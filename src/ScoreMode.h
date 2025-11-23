#ifndef OOP_SCOREMODE_H
#define OOP_SCOREMODE_H

#include "GameMode.h"

class ScoreMode : public GameMode {
public:
  ScoreMode();
  ~ScoreMode() override = default;
  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
};

#endif // OOP_SCOREMODE_H
