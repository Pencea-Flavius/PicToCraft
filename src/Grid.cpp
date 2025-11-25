#include "Grid.h"
#include "MistakesMode.h"
#include "ScoreMode.h"
#include "TimeMode.h"
#include "TorchMode.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <vector>

Grid::Grid()
    : size{}, blocks{}, total_correct_blocks{}, completed_blocks{},
      correct_completed_blocks{}, hints{},
      gameMode(std::make_unique<ScoreMode>()) {}

Grid::Grid(int grid_size, const std::vector<std::vector<bool>> &pattern,
           GameConfig config)
    : size{grid_size}, total_correct_blocks{0}, completed_blocks{0},
      correct_completed_blocks{0} {

  // Base mode
  std::unique_ptr<GameMode> baseMode;
  if (config.baseMode == GameModeType::Mistakes) {
    baseMode = std::make_unique<MistakesMode>();
  } else {
    baseMode = std::make_unique<ScoreMode>();
  }

  // Apply modifiers
  if (config.timeMode) {
    baseMode = std::make_unique<TimeMode>(std::move(baseMode), grid_size);
  }

  if (config.torchMode) {
    baseMode = std::make_unique<TorchMode>(std::move(baseMode));
  }

  gameMode = std::move(baseMode);

  blocks.resize(size);
  for (int i = 0; i < size; i++) {
    blocks[i].reserve(size);
    for (int j = 0; j < size; j++) {
      bool val = (i < static_cast<int>(pattern.size()) &&
                  j < static_cast<int>(pattern[i].size()))
                     ? pattern[i][j]
                     : false;
      if (val)
        total_correct_blocks++;
      blocks[i].emplace_back(val);
    }
  }
  hints = PicrossHints(blocks);
}

Grid::Grid(const Grid &other)
    : size(other.size), blocks(other.blocks),
      total_correct_blocks(other.total_correct_blocks),
      completed_blocks(other.completed_blocks),
      correct_completed_blocks(other.correct_completed_blocks),
      hints(other.hints) {

  if (other.gameMode) {
    gameMode = other.gameMode->clone();
  } else {
    gameMode = std::make_unique<ScoreMode>();
  }
}

Grid &Grid::operator=(const Grid &other) {
  if (this != &other) {
    size = other.size;
    blocks = other.blocks;
    total_correct_blocks = other.total_correct_blocks;

    completed_blocks = other.completed_blocks;
    correct_completed_blocks = other.correct_completed_blocks;
    hints = other.hints;

    if (other.gameMode) {
      gameMode = other.gameMode->clone();
    } else {
      gameMode = std::make_unique<ScoreMode>();
    }
  }
  return *this;
}

Grid::~Grid() = default;

void Grid::load_from_file(const std::string &filename, GameConfig config) {
  std::ifstream file(filename);
  if (!file) {
    std::cout << "Eroare la deschiderea fisierului " << filename << "\n";
    return;
  }

  file >> size;
  blocks.clear();
  blocks.resize(size);
  total_correct_blocks = 0;
  completed_blocks = 0;
  correct_completed_blocks = 0;

  std::string line;
  for (int i = 0; i < size; i++) {
    file >> line;
    blocks[i].clear();
    for (int j = 0; j < size; j++) {
      bool val = (j < static_cast<int>(line.size()) && line[j] == '1');
      if (val)
        total_correct_blocks++;
      blocks[i].emplace_back(val);
    }
  }
  file.close();
  hints = PicrossHints(blocks);

  std::unique_ptr<GameMode> baseMode;
  if (config.baseMode == GameModeType::Mistakes) {
    baseMode = std::make_unique<MistakesMode>();
  } else {
    baseMode = std::make_unique<ScoreMode>();
  }

  // Apply modifiers
  if (config.timeMode) {
    baseMode = std::make_unique<TimeMode>(std::move(baseMode), size);
  }

  if (config.torchMode) {
    baseMode = std::make_unique<TorchMode>(std::move(baseMode));
  }

  gameMode = std::move(baseMode);
}

// Generate random grid
void Grid::generate_random(int grid_size, GameConfig config, double density) {
  size = grid_size;
  blocks.clear();
  blocks.resize(size);
  total_correct_blocks = 0;
  completed_blocks = 0;
  correct_completed_blocks = 0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      bool val = dis(gen) < density;
      if (val)
        total_correct_blocks++;
      blocks[i].emplace_back(val);
    }
  }

  hints = PicrossHints(blocks);

  std::unique_ptr<GameMode> baseMode;
  if (config.baseMode == GameModeType::Mistakes) {
    baseMode = std::make_unique<MistakesMode>();
  } else {
    baseMode = std::make_unique<ScoreMode>();
  }

  // Apply modifiers
  if (config.timeMode) {
    baseMode = std::make_unique<TimeMode>(std::move(baseMode), size);
  }

  if (config.torchMode) {
    baseMode = std::make_unique<TorchMode>(std::move(baseMode));
  }

  gameMode = std::move(baseMode);
}

void Grid::toggle_block(int x, int y) {
  if (x < 0 || y < 0 || x >= size || y >= size) {
    std::cout << "Coordonate invalide!\n";
    return;
  }

  bool was_completed = blocks[x][y].is_completed();
  bool block_correct = blocks[x][y].is_correct();

  blocks[x][y].toggle();

  bool now_completed = blocks[x][y].is_completed();

  if (!was_completed && now_completed) {
    completed_blocks++;
    if (block_correct)
      correct_completed_blocks++;
  } else if (was_completed && !now_completed) {
    completed_blocks--;
    if (block_correct)
      correct_completed_blocks--;
  }

  if (gameMode) {
    gameMode->onBlockToggled(block_correct, now_completed, was_completed);
  }
}

void Grid::update(float deltaTime) {
  if (gameMode) {
    gameMode->update(deltaTime);
  }
}

// Status checks
bool Grid::is_solved() const {
  return completed_blocks == total_correct_blocks &&
         correct_completed_blocks == total_correct_blocks;
}

bool Grid::is_lost() const {
  if (gameMode) {
    return gameMode->isLost();
  }
  return false;
}

int Grid::get_score() const { return gameMode ? gameMode->getScore() : 0; }

int Grid::get_mistakes() const {
  return gameMode ? gameMode->getMistakes() : 0;
}

int Grid::get_max_mistakes() const {
  return gameMode ? gameMode->getMaxMistakes() : 0;
}

bool Grid::shouldDisplayScore() const {
  return gameMode ? gameMode->shouldDisplayScore() : true;
}

bool Grid::is_time_mode() const {
  return gameMode ? gameMode->isTimeMode() : false;
}

void Grid::drawMode(sf::RenderWindow &window) const {
  if (gameMode) {
    gameMode->draw(window);
  }
}

int Grid::get_size() const { return size; }
const Block &Grid::get_block(int x, int y) const { return blocks[x][y]; }

const PicrossHints &Grid::get_hints() const { return hints; }

// Stream output
std::ostream &operator<<(std::ostream &os, const Grid &g) {
  os << g.hints;
  return os;
}