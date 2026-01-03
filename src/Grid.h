//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_GRID_H
#define OOP_GRID_H

#include <iosfwd>
#include <string>
#include <vector>

#include "Block.h"
#include "GameMode.h"
#include "GameConfig.h"
#include "PicrossHints.h"
#include <memory>
#include <SFML/Audio.hpp>

class Grid {
  int size;
  std::vector<std::vector<Block>> blocks;
  int total_correct_blocks;
  int completed_blocks;
  int correct_completed_blocks;
  PicrossHints hints;
  std::unique_ptr<GameMode> gameMode;
  static int totalGridsCreated;
  
public:
  enum class WebDamageResult { None, Damaged, Destroyed };
  
  Grid();

  Grid(int grid_size, const std::vector<std::vector<bool>> &pattern,
       const GameConfig &config = {});

  Grid(const Grid &other);

  Grid &operator=(Grid other);

  friend void swap(Grid &first, Grid &second) noexcept;

  ~Grid();

  void load_from_file(const std::string &filename,
                      const GameConfig &config = {});

  void generate_random(int grid_size, const GameConfig &config = {},
                       double density = 0.5);

  void toggle_block(int x, int y);

  void update(float deltaTime) const;

  [[nodiscard]] bool handleInput(const sf::Event &event,
                   const sf::RenderWindow &window) const;

  [[nodiscard]] bool is_solved() const;

  [[nodiscard]] bool is_lost() const;

  [[nodiscard]] int get_score() const;

  [[nodiscard]] int get_mistakes() const;

  [[nodiscard]] int get_max_mistakes() const;

  [[nodiscard]] bool shouldDisplayScore() const;

  [[nodiscard]] bool shouldShowSurvivalStats() const;

  [[nodiscard]] bool is_time_mode() const;

  [[nodiscard]] int get_size() const;

  [[nodiscard]] const Block &get_block(int x, int y) const;

  [[nodiscard]] const PicrossHints &get_hints() const;

  void drawMode(sf::RenderWindow &window) const;

  void webHint(bool isRow, int line, int index);

  void unwebHint(bool isRow, int line, int index);

  WebDamageResult damageWeb(bool isRow, int line, int index);

  void healWebs(); // Added healWebs
  [[nodiscard]] bool isHintWebbed(bool isRow, int line, int index) const;

  void setRenderer(const class GridRenderer *r) const;

  friend std::ostream &operator<<(std::ostream &os, const Grid &g);
  
  void setSfxVolume(float volume) const {
      if (gameMode) gameMode->setSfxVolume(volume);
      hurtSound.setVolume(volume);
  }

  void damagePlayer(bool playSound = true) const;
  
  // Get game mode (for casting to specific types)
  [[nodiscard]] GameMode* getMode() const { return gameMode.get(); }

private:
  sf::SoundBuffer hurtBuffer;
  mutable sf::Sound hurtSound;
};

#endif // OOP_GRID_H