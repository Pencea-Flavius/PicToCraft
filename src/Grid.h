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
#include "PicrossHints.h"
#include <memory>

class Grid {
  int size;
  std::vector<std::vector<Block>> blocks;
  int total_correct_blocks;
  int completed_blocks;
  int correct_completed_blocks;
  PicrossHints hints;
  std::unique_ptr<GameMode> gameMode;

public:
  Grid();

  Grid(int grid_size, const std::vector<std::vector<bool>> &pattern,
       GameConfig config = {});

  Grid(const Grid &other);
  Grid &operator=(const Grid &other);
  ~Grid();

  void load_from_file(const std::string &filename, GameConfig config = {});

  void generate_random(int grid_size, GameConfig config = {},
                       double density = 0.5);

  void toggle_block(int x, int y);
  void update(float deltaTime);

  [[nodiscard]] bool is_solved() const;

  [[nodiscard]] bool is_lost() const;

  [[nodiscard]] int get_score() const;
  [[nodiscard]] int get_mistakes() const;
  [[nodiscard]] int get_max_mistakes() const;
  [[nodiscard]] bool shouldDisplayScore() const;
  [[nodiscard]] bool is_time_mode() const;
  [[nodiscard]] bool is_dark_mode() const;
  [[nodiscard]] int get_size() const;
  [[nodiscard]] const Block &get_block(int x, int y) const;
  [[nodiscard]] const PicrossHints &get_hints() const;
  void drawMode(sf::RenderWindow &window) const;

  friend std::ostream &operator<<(std::ostream &os, const Grid &g);
};

#endif // OOP_GRID_H