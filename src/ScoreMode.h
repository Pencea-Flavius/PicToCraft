#ifndef OOP_SCOREMODE_H
#define OOP_SCOREMODE_H

#include "GameMode.h"

class ScoreMode : public GameMode {
public:
  ScoreMode() = default;
  ~ScoreMode() override = default;
  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override;
  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
};

#endif // OOP_SCOREMODE_H
