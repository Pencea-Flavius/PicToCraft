#include "ScoreMode.h"

ScoreMode::ScoreMode() = default;

bool ScoreMode::isLost() const {
  return false;
}

bool ScoreMode::shouldDisplayScore() const { return true; }
