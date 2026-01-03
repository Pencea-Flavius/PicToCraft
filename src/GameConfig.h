#ifndef OOP_GAMECONFIG_H
#define OOP_GAMECONFIG_H

enum class GameModeType { Score, Mistakes, Time, Torch, Spiders };

struct GameConfig {
  GameModeType baseMode = GameModeType::Score;
  bool timeMode = false;
  bool torchMode = false;
  bool spidersMode = false;
  bool discoFeverMode = false;
  bool endermanMode = false;
  bool alchemyMode = false;
  bool backgroundMovement = true;

  float masterVolume = 1.0f;
  float musicVolume = 1.0f;
  float sfxVolume = 1.0f;

  [[nodiscard]] int calculateBonus() const;
};

#endif // OOP_GAMECONFIG_H
