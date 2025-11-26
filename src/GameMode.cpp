#include "GameMode.h"
#include <iostream>

int GameMode::totalGameModesCreated = 0;
int GameMode::activeGameModes = 0;

GameMode::GameMode() : score(1000), mistakes(0) {
  totalGameModesCreated++;
  activeGameModes++;
  std::cout << "GameMode created. Total: " << totalGameModesCreated
            << ", Active: " << activeGameModes << "\n";
}

GameMode::~GameMode() {
  activeGameModes--;
  std::cout << "GameMode destroyed. Active: " << activeGameModes << "\n";
}

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
  } else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      score -= 300;
    }
  }

  if (score < 0)
    score = 0;
}
