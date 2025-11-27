#include "MistakesMode.h"
#include "Exceptions.h"

MistakesMode::MistakesMode() : hurtSound(hurtBuffer) {
  if (!hurtBuffer.loadFromFile("assets/sound/hurt.mp3")) {
    throw AssetLoadException("assets/sound/hurt.mp3", "Sound");
  }
}

MistakesMode::MistakesMode(const MistakesMode &other)
    : GameMode(other), hurtBuffer(other.hurtBuffer), hurtSound(hurtBuffer) {
  mistakes = other.mistakes;
  score = other.score;
}

void MistakesMode::onBlockToggled(bool isCorrect, bool isCompleted,
                                  bool wasCompleted) {
  // Call base scoring logic
  GameMode::onBlockToggled(isCorrect, isCompleted, wasCompleted);

  // Add mistake logic
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

bool MistakesMode::isLost() const { return mistakes >= 3; }

int MistakesMode::getMaxMistakes() const { return 3; }

bool MistakesMode::shouldDisplayScore() const { return false; }

std::unique_ptr<GameMode> MistakesMode::clone() const {
  return std::make_unique<MistakesMode>(*this);
}
