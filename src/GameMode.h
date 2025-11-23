#ifndef OOP_GAMEMODE_H
#define OOP_GAMEMODE_H

enum class GameModeType { Score, Mistakes, Time };

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
  virtual void update(float deltaTime) {}

  [[nodiscard]] int getScore() const;
  [[nodiscard]] int getMistakes() const;

  void reset();
};

#endif // OOP_GAMEMODE_H
