#include "ScoreMode.h"

ScoreMode::ScoreMode() = default;

bool ScoreMode::isLost() const { return false; }

bool ScoreMode::shouldDisplayScore() const { return true; }

std::unique_ptr<GameMode> ScoreMode::clone() const {
  return std::make_unique<ScoreMode>(*this);
}
