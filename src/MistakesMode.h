#ifndef OOP_MISTAKESMODE_H
#define OOP_MISTAKESMODE_H

#include "GameMode.h"

class MistakesMode : public GameMode {
public:
  MistakesMode() = default;
  ~MistakesMode() override = default;
  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override;
  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] int getMaxMistakes() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
};

#endif // OOP_MISTAKESMODE_H
