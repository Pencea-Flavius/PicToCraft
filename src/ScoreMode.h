#ifndef OOP_SCOREMODE_H
#define OOP_SCOREMODE_H

#include "GameMode.h"

class ScoreMode : public GameMode {
public:
  ScoreMode();
  ~ScoreMode() override = default;

  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
  [[nodiscard]] bool shouldShowSurvivalStats() const override { return false; }
  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  void print(std::ostream &os) const override {
    os << "ScoreMode (Score: " << score << ")";
  }
  [[nodiscard]] std::string getName() const override { return "Score Mode"; }
};

#endif // OOP_SCOREMODE_H
