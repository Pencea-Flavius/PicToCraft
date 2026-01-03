#ifndef GAMEMODEFACTORY_H
#define GAMEMODEFACTORY_H

#include "GameMode.h"
#include "GameConfig.h"
#include <memory>

class GameModeFactory {
public:
  static std::unique_ptr<GameMode> createGameMode(const GameConfig &config,
                                                  int gridSize);
};

#endif // GAMEMODEFACTORY_H
