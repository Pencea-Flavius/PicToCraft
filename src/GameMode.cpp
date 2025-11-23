#include "GameMode.h"

GameMode::GameMode() : score(1000), mistakes(0) {}

int GameMode::getScore() const { return score; }

int GameMode::getMistakes() const { return mistakes; }

void GameMode::reset() {
  score = 1000;
  mistakes = 0;
}

void GameMode::onBlockToggled(bool isCorrect, bool isCompleted,
                              bool wasCompleted) {
  if (!wasCompleted && isCompleted) {
    if (isCorrect) {
      score += 200;
    } else {
      score -= 100;
    }
  }
  else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      score -= 300;
    }
  }

  if (score < 0)
    score = 0;
}
