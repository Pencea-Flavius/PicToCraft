#include "GameMode.h"

GameMode::GameMode() : score(1000), mistakes(0) {}

int GameMode::getScore() const { return score; }

int GameMode::getMistakes() const { return mistakes; }

void GameMode::reset() {
  score = 1000;
  mistakes = 0;
}
