#include "GameModeFactory.h"
#include "DiscoFeverMode.h"
#include "EndermanMode.h"
#include "MistakesMode.h"
#include "ScoreMode.h"
#include "SpidersMode.h"
#include "TimeMode.h"
#include "TorchMode.h"
#include "AlchemyMode.h"

#include "Exceptions.h"

std::unique_ptr<GameMode>
GameModeFactory::createGameMode(const GameConfig &config, int gridSize) {
  std::unique_ptr<GameMode> baseMode;


  if (config.baseMode == GameModeType::Mistakes) {
    baseMode = std::make_unique<MistakesMode>();
  } else if (config.baseMode == GameModeType::Score) {
    baseMode = std::make_unique<ScoreMode>();
  } else {
    throw InvalidGameModeException("Invalid base game mode selected");
  }


  if (config.timeMode) {
    baseMode = std::make_unique<TimeMode>(std::move(baseMode), gridSize);
  }

  if (config.spidersMode) {
    baseMode = std::make_unique<SpidersMode>(std::move(baseMode));
  }

  if (config.alchemyMode) {
    baseMode = std::make_unique<AlchemyMode>(std::move(baseMode));
  }

  if (config.torchMode) {
    baseMode = std::make_unique<TorchMode>(std::move(baseMode));
  }

  if (config.discoFeverMode) {
    baseMode = std::make_unique<DiscoFeverMode>(std::move(baseMode));
  }

  if (config.endermanMode) {
    baseMode = std::make_unique<EndermanMode>(std::move(baseMode));
  }

  return baseMode;
}