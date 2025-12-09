
#include "TimeMode.h"
#include "Exceptions.h"

TimeMode::TimeMode(std::unique_ptr<GameMode> mode, int gridSize)
    : GameModeDecorator(std::move(mode)), decayTimer(0.0f), timeLeft(0.0f),
      totalTime(static_cast<float>(gridSize) * 15.0f), hurtSound(hurtBuffer) {

  maxHearts = 20;

  if (gridSize <= 5) {
    decayInterval = 5.0f;
  } else if (gridSize <= 8) {
    decayInterval = 10.0f;
  } else if (gridSize <= 12) {
    decayInterval = 20.0f;
  } else {
    decayInterval = 60.0f;
  }

  mistakes = 0;
  timeLeft = totalTime;

  if (!hurtBuffer.loadFromFile("assets/sound/hurt.mp3")) {
    throw AssetLoadException("assets/sound/hurt.mp3", "Sound");
  }
}

void TimeMode::onBlockToggled(bool isCorrect, bool isCompleted,
                              bool wasCompleted) {
  GameModeDecorator::onBlockToggled(isCorrect, isCompleted, wasCompleted);

  if (!wasCompleted && isCompleted) {
    if (!isCorrect) {
      mistakes++;
      hurtSound.play();
    }
  } else if (wasCompleted && !isCompleted) {
    if (isCorrect) {
      mistakes++;
      hurtSound.play();
    }
  }
}

void TimeMode::update(float deltaTime) {
  if (isLost())
    return;

  decayTimer += deltaTime;
  if (decayTimer >= decayInterval) {
    decayTimer -= decayInterval;
    mistakes++;
    hurtSound.play();
  }
}

bool TimeMode::isLost() const {
  return mistakes >= maxHearts;
}

int TimeMode::getMaxMistakes() const { return maxHearts; }

bool TimeMode::shouldDisplayScore() const {
  return false; // Display hearts instead
}

int TimeMode::getMistakes() const { return mistakes; }

std::unique_ptr<GameMode> TimeMode::clone() const {
  auto clonedWrapped = wrappedMode ? wrappedMode->clone() : nullptr;
  auto newMode = std::make_unique<TimeMode>(std::move(clonedWrapped), 0);
  newMode->maxHearts = this->maxHearts;
  newMode->decayTimer = this->decayTimer;
  newMode->decayInterval = this->decayInterval;
  newMode->mistakes = this->mistakes;
  newMode->score = this->score;
  return newMode;
}
