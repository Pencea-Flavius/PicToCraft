#ifndef OOP_GAMEMODE_H
#define OOP_GAMEMODE_H

#include <memory>

enum class GameModeType { Score, Mistakes, Time, Torch };

struct GameConfig {
  GameModeType baseMode = GameModeType::Score;
  bool timeMode = false;
  bool torchMode = false;
};

namespace sf {
class RenderWindow;
}

class GameMode {
protected:
  int score;
  int mistakes;

public:
  GameMode();
  virtual ~GameMode() = default;

  virtual void onBlockToggled(bool isCorrect, bool isCompleted,
                              bool wasCompleted);
  [[nodiscard]] virtual bool isLost() const = 0;
  [[nodiscard]] virtual int getMaxMistakes() const { return 0; }
  [[nodiscard]] virtual bool shouldDisplayScore() const = 0;
  [[nodiscard]] virtual bool isTimeMode() const { return false; }
  [[nodiscard]] virtual bool isTorchMode() const { return false; }
  virtual void update(float deltaTime) {}
  virtual void draw(class sf::RenderWindow &window) const {}

  [[nodiscard]] virtual int getScore() const;
  [[nodiscard]] virtual int getMistakes() const;

  [[nodiscard]] virtual std::unique_ptr<GameMode> clone() const = 0;

  void reset();
};

class GameModeDecorator : public GameMode {
protected:
  std::unique_ptr<GameMode> wrappedMode;

public:
  explicit GameModeDecorator(std::unique_ptr<GameMode> mode)
      : wrappedMode(std::move(mode)) {}

  ~GameModeDecorator() override = default;

  [[nodiscard]] std::unique_ptr<GameMode> clone() const override = 0;

  void onBlockToggled(bool isCorrect, bool isCompleted,
                      bool wasCompleted) override {
    if (wrappedMode) {
      wrappedMode->onBlockToggled(isCorrect, isCompleted, wasCompleted);
    }
  }

  [[nodiscard]] bool isLost() const override {
    return wrappedMode ? wrappedMode->isLost() : false;
  }

  [[nodiscard]] int getMaxMistakes() const override {
    return wrappedMode ? wrappedMode->getMaxMistakes() : 0;
  }

  [[nodiscard]] bool shouldDisplayScore() const override {
    return wrappedMode ? wrappedMode->shouldDisplayScore() : false;
  }

  [[nodiscard]] bool isTimeMode() const override {
    return wrappedMode ? wrappedMode->isTimeMode() : false;
  }

  [[nodiscard]] bool isTorchMode() const override {
    return wrappedMode ? wrappedMode->isTorchMode() : false;
  }

  void update(float deltaTime) override {
    if (wrappedMode) {
      wrappedMode->update(deltaTime);
    }
  }

  void draw(sf::RenderWindow &window) const override {
    if (wrappedMode) {
      wrappedMode->draw(window);
    }
  }

  [[nodiscard]] int getScore() const override {
    return wrappedMode ? wrappedMode->getScore() : 0;
  }

  [[nodiscard]] int getMistakes() const override {
    return wrappedMode ? wrappedMode->getMistakes() : 0;
  }
};

#endif // OOP_GAMEMODE_H
