#include "Grid.h"
#include "Exceptions.h"
#include "GameModeFactory.h"
#include "ScoreMode.h"
#include "TimeMode.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <vector>

int Grid::totalGridsCreated = 0;

Grid::Grid()
    : size{}, blocks{}, total_correct_blocks{}, completed_blocks{},
      correct_completed_blocks{}, hints{},
      gameMode(std::make_unique<ScoreMode>()) {
  totalGridsCreated++;
}

Grid::Grid(int grid_size, const std::vector<std::vector<bool>> &pattern,
           GameConfig config)
    : size{grid_size}, total_correct_blocks{0}, completed_blocks{0},
      correct_completed_blocks{0},
      gameMode(GameModeFactory::createGameMode(config, grid_size)) {
  totalGridsCreated++;
  if (gameMode)
    gameMode->setGrid(this);

  if (gameMode)
    std::cout << "Created Grid with mode: " << *gameMode << "\n";

  std::vector<std::vector<bool>> bool_grid(size, std::vector<bool>(size));
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
      bool_grid[i][j] = val;
    }
  }
  hints = PicrossHints(bool_grid);
}

Grid::Grid(const Grid &other)
    : size(other.size), blocks(other.blocks),
      total_correct_blocks(other.total_correct_blocks),
      completed_blocks(other.completed_blocks),
      correct_completed_blocks(other.correct_completed_blocks),
      hints(other.hints) {
  totalGridsCreated++;

  if (other.gameMode) {
    gameMode = other.gameMode->clone();
  } else {
    gameMode = std::make_unique<ScoreMode>();
  }
  if (gameMode)
    gameMode->setGrid(this);
}

void swap(Grid &first, Grid &second) noexcept {
  using std::swap;
  swap(first.size, second.size);
  swap(first.blocks, second.blocks);
  swap(first.total_correct_blocks, second.total_correct_blocks);
  swap(first.completed_blocks, second.completed_blocks);
  swap(first.correct_completed_blocks, second.correct_completed_blocks);
  swap(first.hints, second.hints);
  swap(first.gameMode, second.gameMode);

  if (first.gameMode)
    first.gameMode->setGrid(&first);
  if (second.gameMode)
    second.gameMode->setGrid(&second);
}

Grid &Grid::operator=(Grid other) {
  swap(*this, other);
  return *this;
}

Grid::~Grid() = default;

void Grid::load_from_file(const std::string &filename, GameConfig config) {
  std::ifstream file(filename);
  if (!file) {
    throw FileLoadException(filename);
  }

  file >> size;
  blocks.clear();
  blocks.resize(size);
  total_correct_blocks = 0;
  completed_blocks = 0;
  correct_completed_blocks = 0;

  std::vector<std::vector<bool>> bool_grid(size, std::vector<bool>(size));
  std::string line;
  for (int i = 0; i < size; i++) {
    file >> line;
    blocks[i].clear();
    for (int j = 0; j < size; j++) {
      bool val = (j < static_cast<int>(line.size()) && line[j] == '1');
      if (val)
        total_correct_blocks++;
      blocks[i].emplace_back(val);
      bool_grid[i][j] = val;
    }
  }
  file.close();
  hints = PicrossHints(bool_grid);

  gameMode = GameModeFactory::createGameMode(config, size);
  if (gameMode)
    gameMode->setGrid(this);
  if (gameMode)
    std::cout << "Loaded Grid with mode: " << *gameMode << "\n";
}

// Generate random grid
void Grid::generate_random(int grid_size, GameConfig config, double density) {
  size = grid_size;
  blocks.clear();
  blocks.resize(size);
  total_correct_blocks = 0;
  completed_blocks = 0;
  correct_completed_blocks = 0;

  std::vector<std::vector<bool>> bool_grid(size, std::vector<bool>(size));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      bool val = dis(gen) < density;
      if (val)
        total_correct_blocks++;
      blocks[i].emplace_back(val);
      bool_grid[i][j] = val;
    }
  }

  hints = PicrossHints(bool_grid);

  gameMode = GameModeFactory::createGameMode(config, size);
  if (gameMode)
    gameMode->setGrid(this);
  if (gameMode)
    std::cout << "Generated Random Grid with mode: " << *gameMode << "\n";
}

void Grid::toggle_block(int x, int y) {
  if (x < 0 || y < 0 || x >= size || y >= size) {
    throw InvalidGridException("Coordinates out of bounds: (" +
                               std::to_string(x) + ", " + std::to_string(y) +
                               ")");
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

void Grid::update(float deltaTime) const {
  if (gameMode) {
    gameMode->update(deltaTime);
  }
}

bool Grid::handleInput(const sf::Event &event,
                       const sf::RenderWindow &window) const {
  if (gameMode) {
    return gameMode->handleInput(event, window);
  }
  return false;
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
  GameMode *current = gameMode.get();
  while (current) {
    if (dynamic_cast<TimeMode *>(current)) {
      return true;
    }
    if (const auto *decorator =
            dynamic_cast<const GameModeDecorator *>(current)) {
      current = decorator->getWrappedMode();
    } else {
      break;
    }
  }
  return false;
}

void Grid::drawMode(sf::RenderWindow &window) const {
  if (gameMode) {
    gameMode->draw(window);
  }
}

int Grid::get_size() const { return size; }
const Block &Grid::get_block(int x, int y) const { return blocks[x][y]; }

const PicrossHints &Grid::get_hints() const { return hints; }

void Grid::webHint(bool isRow, int line, int index) {
  hints.setWebHealth(isRow, line, index, 10); // Set initial health to 10
}

void Grid::unwebHint(bool isRow, int line, int index) {
  hints.setWebHealth(isRow, line, index, 0);
}

void Grid::damageWeb(bool isRow, int line, int index) {
  int currentHealth = hints.getWebHealth(isRow, line, index);
  if (currentHealth > 0) {
    hints.setWebHealth(isRow, line, index, currentHealth - 1);
    if (currentHealth - 1 <= 0) {
      unwebHint(isRow, line, index);
    }
  }
}

void Grid::healWebs() {
  const auto &rowHints = hints.get_row_hints();
  for (size_t i = 0; i < rowHints.size(); ++i) {
    for (size_t j = 0; j < rowHints[i].size(); ++j) {
      if (hints.isWebbed(true, static_cast<int>(i), static_cast<int>(j))) {
        int health =
            hints.getWebHealth(true, static_cast<int>(i), static_cast<int>(j));
        if (health < 10) {
          hints.setWebHealth(true, static_cast<int>(i), static_cast<int>(j),
                             10); // Fully heal
        }
      }
    }
  }

  const auto &colHints = hints.get_col_hints();
  for (size_t j = 0; j < colHints.size(); ++j) {
    for (size_t i = 0; i < colHints[j].size(); ++i) {
      if (hints.isWebbed(false, static_cast<int>(j), static_cast<int>(i))) {
        int health =
            hints.getWebHealth(false, static_cast<int>(j), static_cast<int>(i));
        if (health < 10) {
          hints.setWebHealth(false, static_cast<int>(j), static_cast<int>(i),
                             10);
        }
      }
    }
  }
}

bool Grid::isHintWebbed(bool isRow, int line, int index) const {
  return hints.isWebbed(isRow, line, index);
}

void Grid::setRenderer(const GridRenderer *r) const {
  if (gameMode)
    gameMode->setRenderer(r);
}

// Stream output
std::ostream &operator<<(std::ostream &os, const Grid &g) {
  os << g.hints;
  return os;
}