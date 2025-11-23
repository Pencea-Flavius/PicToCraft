#include "Grid.h"
#include "MistakesMode.h"
#include "ScoreMode.h"
#include "TimeMode.h"

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
           GameModeType mode)
    : size{grid_size}, total_correct_blocks{0}, completed_blocks{0},
      correct_completed_blocks{0} {

  if (mode == GameModeType::Score) {
    gameMode = std::make_unique<ScoreMode>();
  } else if (mode == GameModeType::Mistakes) {
    gameMode = std::make_unique<MistakesMode>();
  } else {
    gameMode = std::make_unique<TimeMode>(grid_size);
  }

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
  if (dynamic_cast<ScoreMode *>(other.gameMode.get())) {
    gameMode = std::make_unique<ScoreMode>(
        *dynamic_cast<ScoreMode *>(other.gameMode.get()));
  } else if (dynamic_cast<MistakesMode *>(other.gameMode.get())) {
    gameMode = std::make_unique<MistakesMode>(
        *dynamic_cast<MistakesMode *>(other.gameMode.get()));
  } else {
    gameMode = std::make_unique<TimeMode>(
        *dynamic_cast<TimeMode *>(other.gameMode.get()));
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

    if (dynamic_cast<ScoreMode *>(other.gameMode.get())) {
      gameMode = std::make_unique<ScoreMode>(
          *dynamic_cast<ScoreMode *>(other.gameMode.get()));
    } else if (dynamic_cast<MistakesMode *>(other.gameMode.get())) {
      gameMode = std::make_unique<MistakesMode>(
          *dynamic_cast<MistakesMode *>(other.gameMode.get()));
    } else {
      gameMode = std::make_unique<TimeMode>(
          *dynamic_cast<TimeMode *>(other.gameMode.get()));
    }
  }
  return *this;
}

Grid::~Grid() = default;

void Grid::load_from_file(const std::string &filename, GameModeType mode) {
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

  if (mode == GameModeType::Score) {
    gameMode = std::make_unique<ScoreMode>();
  } else if (mode == GameModeType::Mistakes) {
    gameMode = std::make_unique<MistakesMode>();
  } else {
    gameMode = std::make_unique<TimeMode>(size);
  }
}

// Generate random grid
void Grid::generate_random(int grid_size, GameModeType mode, double density) {
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

  if (mode == GameModeType::Score) {
    gameMode = std::make_unique<ScoreMode>();
  } else if (mode == GameModeType::Mistakes) {
    gameMode = std::make_unique<MistakesMode>();
  } else {
    gameMode = std::make_unique<TimeMode>(size);
  }
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

int Grid::get_size() const { return size; }
const Block &Grid::get_block(int x, int y) const { return blocks[x][y]; }

const PicrossHints &Grid::get_hints() const { return hints; }

// Stream output
std::ostream &operator<<(std::ostream &os, const Grid &g) {
  os << g.hints;
  return os;
}