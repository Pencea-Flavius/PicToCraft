#include "DiscoFeverMode.h"
#include "GridRenderer.h"

DiscoFeverMode::DiscoFeverMode(std::unique_ptr<GameMode> mode)
    : GameModeDecorator(std::move(mode)) {}

std::unique_ptr<GameMode> DiscoFeverMode::clone() const {
  return std::make_unique<DiscoFeverMode>(wrappedMode->clone());
}

void DiscoFeverMode::setRenderer(const GridRenderer *r) {
  if (r) {
    const_cast<GridRenderer *>(r)->setDiscoFeverMode(true);
  }
  GameModeDecorator::setRenderer(r);
}
