#ifndef OOP_MISTAKESMODE_H
#define OOP_MISTAKESMODE_H

#include "GameMode.h"
#include <SFML/Audio.hpp>

class MistakesMode : public GameMode {
public:
  MistakesMode();
  MistakesMode(const MistakesMode &other);
  ~MistakesMode() override = default;
  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override;
  [[nodiscard]] bool isLost() const override;
  [[nodiscard]] int getMaxMistakes() const override;
  [[nodiscard]] bool shouldDisplayScore() const override;
  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  void print(std::ostream &os) const override { os << "MistakesMode"; }
  [[nodiscard]] std::string getName() const override { return "Mistakes Mode"; }

private:
  sf::SoundBuffer hurtBuffer;
  sf::Sound hurtSound;
};

#endif // OOP_MISTAKESMODE_H
