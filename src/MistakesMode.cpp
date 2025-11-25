#include "MistakesMode.h"

void MistakesMode::onBlockToggled(bool isCorrect, bool isCompleted,
                                  bool wasCompleted) {
  // Call base scoring logic
  GameMode::onBlockToggled(isCorrect, isCompleted, wasCompleted);

  // Add mistake logic
  if (!wasCompleted && isCompleted) {
    if (!isCorrect) {
      mistakes++;
    }
  }
}

bool MistakesMode::isLost() const { return mistakes >= 3; }

int MistakesMode::getMaxMistakes() const { return 3; }

bool MistakesMode::shouldDisplayScore() const { return false; }

std::unique_ptr<GameMode> MistakesMode::clone() const {
  return std::make_unique<MistakesMode>(*this);
}
