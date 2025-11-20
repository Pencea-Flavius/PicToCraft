#include "ScoreMode.h"

void ScoreMode::onBlockToggled(bool isCorrect, bool isCompleted,
                               bool wasCompleted) {
  // If we just completed a block
  if (!wasCompleted && isCompleted) {
    if (isCorrect) {
      score += 200;
    } else {
      score -= 100;
    }
  }
  // If we un-completed a block (toggled off)
  else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      score -= 300;
    }
  }

  if (score < 0)
    score = 0;
}

bool ScoreMode::isLost() const {
  return false; // Score mode doesn't lose by mistakes
}

bool ScoreMode::shouldDisplayScore() const { return true; }
