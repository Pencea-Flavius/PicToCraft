//
// Created by zzfla on 11/8/2025.
//

#include "Grid.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <ostream>

// Default constructor
Grid::Grid()
    : size{}, blocks{}, total_correct_blocks{}, completed_blocks{}, correct_completed_blocks{},
      hints{}, mistakes{}, score{}, score_mode{false} {}

// Construct grid from a boolean pattern
Grid::Grid(int grid_size, const std::vector<std::vector<bool>>& pattern, bool use_score_mode)
    : size{grid_size}, total_correct_blocks{0}, completed_blocks{0}, correct_completed_blocks{0},
      mistakes{0}, score{1000}, score_mode{use_score_mode} {
    blocks.resize(size);
    for (int i = 0; i < size; i++) {
        blocks[i].reserve(size);
        for (int j = 0; j < size; j++) {
            bool val = (i < static_cast<int>(pattern.size()) &&
                        j < static_cast<int>(pattern[i].size())) ? pattern[i][j] : false;
            if (val) total_correct_blocks++;
            blocks[i].emplace_back(val);
        }
    }
    hints = PicrossHints(blocks);
}

// Copy constructor
Grid::Grid(const Grid& other)
    : size(other.size), blocks(other.blocks), total_correct_blocks(other.total_correct_blocks),
      completed_blocks(other.completed_blocks), correct_completed_blocks(other.correct_completed_blocks),
      hints(other.hints), mistakes(other.mistakes), score(other.score), score_mode(other.score_mode) {}

// Copy assignment
Grid& Grid::operator=(const Grid& other) {
    if (this != &other) {
        size = other.size;
        blocks = other.blocks;
        total_correct_blocks = other.total_correct_blocks;
        completed_blocks = other.completed_blocks;
        correct_completed_blocks = other.correct_completed_blocks;
        hints = other.hints;
        mistakes = other.mistakes;
        score = other.score;
        score_mode = other.score_mode;
    }
    return *this;
}

// Destructor
Grid::~Grid() = default;

// Load grid from file
void Grid::load_from_file(const std::string& filename, bool use_score_mode) {
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
            if (val) total_correct_blocks++;
            blocks[i].emplace_back(val);
        }
    }
    file.close();
    hints = PicrossHints(blocks);
    mistakes = 0;
    score = 1000;
    score_mode = use_score_mode;
}

// Generate random grid
void Grid::generate_random(int grid_size, bool use_score_mode, double density) {
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
            if (val) total_correct_blocks++;
            blocks[i].emplace_back(val);
        }
    }

    hints = PicrossHints(blocks);
    mistakes = 0;
    score = 1000;
    score_mode = use_score_mode;
}

// Toggle a block at coordinates (x, y)
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
        if (block_correct) correct_completed_blocks++;
    } else if (was_completed && !now_completed) {
        completed_blocks--;
        if (block_correct) correct_completed_blocks--;
    }

    if (score_mode) {
        if (now_completed && block_correct) {
            score += 200;
        } else if (now_completed && !block_correct) {
            score -= 100;
        } else if (block_correct) {
            score -= 300;
        }
        if (score < 0) score = 0;
    } else {
        if ((now_completed && !block_correct) || (!now_completed && block_correct)) {
            mistakes++;
        }
    }
}

// Status checks
bool Grid::is_solved() const {
    return completed_blocks == total_correct_blocks &&
           correct_completed_blocks == total_correct_blocks;
}

bool Grid::is_lost() const {
    return !score_mode && mistakes >= 3;
}

// Getters
int Grid::get_score() const { return score; }
int Grid::get_mistakes() const { return mistakes; }
bool Grid::get_score_mode() const { return score_mode; }
int Grid::get_size() const { return size; }

// Stream output
std::ostream& operator<<(std::ostream& os, const Grid& g) {
    os << g.hints;
    return os;
}