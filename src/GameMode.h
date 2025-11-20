#ifndef OOP_GAMEMODE_H
#define OOP_GAMEMODE_H

class GameMode {
protected:
  int score;
  int mistakes;

public:
  GameMode();
  virtual ~GameMode() = default;

  virtual void onBlockToggled(bool isCorrect, bool isCompleted,
                              bool wasCompleted) = 0;
  [[nodiscard]] virtual bool isLost() const = 0;
  [[nodiscard]] virtual bool shouldDisplayScore() const = 0;

  [[nodiscard]] int getScore() const;
  [[nodiscard]] int getMistakes() const;

  void reset();
};

#endif // OOP_GAMEMODE_H
