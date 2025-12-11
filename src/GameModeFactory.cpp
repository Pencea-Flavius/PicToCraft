#include "GameModeFactory.h"
#include "DementiaMode.h"
#include "EndermanMode.h"
#include "MistakesMode.h"
#include "ScoreMode.h"
#include "SpidersMode.h"
#include "TimeMode.h"
#include "TorchMode.h"

#include "Exceptions.h"

std::unique_ptr<GameMode>
GameModeFactory::createGameMode(const GameConfig &config, int gridSize) {
    std::unique_ptr<GameMode> baseMode;

    // Create base mode
    if (config.baseMode == GameModeType::Mistakes) {
        baseMode = std::make_unique<MistakesMode>();
    } else if (config.baseMode == GameModeType::Score) {
        baseMode = std::make_unique<ScoreMode>();
    } else {
        throw GameException("Invalid base game mode selected");
    }

    // Apply decorators
    if (config.timeMode) {
        baseMode = std::make_unique<TimeMode>(std::move(baseMode), gridSize);
    }

    if (config.spidersMode) {
        baseMode = std::make_unique<SpidersMode>(std::move(baseMode));
    }

    if (config.torchMode) {
        baseMode = std::make_unique<TorchMode>(std::move(baseMode));
    }

    if (config.dementiaMode) {
        baseMode = std::make_unique<DementiaMode>(std::move(baseMode));
    }

    if (config.endermanMode) {
        baseMode = std::make_unique<EndermanMode>(std::move(baseMode));
    }

    return baseMode;
}