#include "GameConfig.h"

int GameConfig::calculateBonus() const {
  int bonus = 0;
  if (timeMode) bonus += 2000;
  if (torchMode) bonus += 2000;
  if (spidersMode) bonus += 2000;
  if (discoFeverMode) bonus += 2000;
  if (endermanMode) bonus += 2000;
  if (alchemyMode) bonus += 2000;
  return bonus;
}
