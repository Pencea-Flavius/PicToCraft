#include "TimeMode.h"

TimeMode::TimeMode(std::unique_ptr<GameMode> mode, int gridSize)
    : GameModeDecorator(std::move(mode)), decayTimer(0.0f) {

  if (gridSize <= 5) {
    maxHearts = 6;
  } else if (gridSize <= 8) {
    maxHearts = 10;
  } else if (gridSize <= 12) {
    maxHearts = 15;
  } else {
    maxHearts = 20;
  }

  mistakes = 0; // Starts with 0 damage
}

void TimeMode::onBlockToggled(bool isCorrect, bool isCompleted,
                              bool wasCompleted) {
  GameModeDecorator::onBlockToggled(isCorrect, isCompleted, wasCompleted);

  if (!wasCompleted && isCompleted) {
    if (!isCorrect) {
      mistakes++;
    }
  } else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      mistakes++;
    }
  }
}

void TimeMode::update(float deltaTime) {
  if (isLost())
    return;

  decayTimer += deltaTime;
  if (decayTimer >= DECAY_INTERVAL) {
    decayTimer -= DECAY_INTERVAL;
    // Lose half a heart every 10 seconds
    mistakes++;
  }
}

bool TimeMode::isLost() const {
  // Lost if damage taken equals max health
  return mistakes >= maxHearts;
}

int TimeMode::getMaxMistakes() const { return maxHearts; }

bool TimeMode::shouldDisplayScore() const {
  return false; // Display hearts instead
}

bool TimeMode::isTimeMode() const { return true; }

int TimeMode::getMistakes() const { return mistakes; }

std::unique_ptr<GameMode> TimeMode::clone() const {
  auto clonedWrapped = wrappedMode ? wrappedMode->clone() : nullptr;
  // Use 0 as gridSize, we'll overwrite maxHearts anyway
  auto newMode = std::make_unique<TimeMode>(std::move(clonedWrapped), 0);
  newMode->maxHearts = this->maxHearts;
  newMode->decayTimer = this->decayTimer;
  newMode->mistakes = this->mistakes;
  newMode->score = this->score;
  return newMode;
}
