#include "MistakesMode.h"

void MistakesMode::onBlockToggled(bool isCorrect, bool isCompleted,
                                  bool wasCompleted) {
  if ((isCompleted && !isCorrect) || (!isCompleted && isCorrect)) {
    mistakes++;
  }

  if (!wasCompleted && isCompleted) {
    if (isCorrect) {
      score += 200;
    } else {
      score -= 100;
    }
  } else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      score -= 300;
    }
  }
  if (score < 0)
    score = 0;
}

bool MistakesMode::isLost() const { return mistakes >= 3; }

bool MistakesMode::shouldDisplayScore() const { return false; }
