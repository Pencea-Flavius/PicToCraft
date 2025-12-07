#include "DementiaMode.h"
#include "GridRenderer.h"

DementiaMode::DementiaMode(std::unique_ptr<GameMode> mode)
    : GameModeDecorator(std::move(mode)) {}

std::unique_ptr<GameMode> DementiaMode::clone() const {
  return std::make_unique<DementiaMode>(wrappedMode->clone());
}

void DementiaMode::setRenderer(const GridRenderer *r) {
  if (r) {
    const_cast<GridRenderer *>(r)->setDementiaMode(true);
  }
  GameModeDecorator::setRenderer(r);
}
