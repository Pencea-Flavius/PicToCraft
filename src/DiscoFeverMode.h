#ifndef DISCOFEVERMODE_H
#define DISCOFEVERMODE_H

#include "GameMode.h"

class DiscoFeverMode : public GameModeDecorator {
public:
  explicit DiscoFeverMode(std::unique_ptr<GameMode> mode);
  ~DiscoFeverMode() override = default;

  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  [[nodiscard]] std::string getName() const override {
    return "Disco Fever Mode";
  }

  void print(std::ostream &os) const override {
    os << "DiscoFeverMode + ";
    GameModeDecorator::print(os);
  }

  void setRenderer(const GridRenderer *r) override;
};

#endif // DISCOFEVERMODE_H
