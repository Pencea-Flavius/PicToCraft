#ifndef DEMENTIAMODE_H
#define DEMENTIAMODE_H

#include "GameMode.h"

class DementiaMode : public GameModeDecorator {
public:
  explicit DementiaMode(std::unique_ptr<GameMode> mode);
  ~DementiaMode() override = default;

  [[nodiscard]] std::unique_ptr<GameMode> clone() const override;
  [[nodiscard]] std::string getName() const override { return "Dementia Mode"; }

  void setRenderer(const class GridRenderer *r) override;
};

#endif
