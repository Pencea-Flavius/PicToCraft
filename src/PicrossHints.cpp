//
// Created by zzfla on 11/8/2025.
//

#include "PicrossHints.h"
#include <ostream>
#include <algorithm>

PicrossHints::PicrossHints() : row_hints{}, col_hints{} {}

PicrossHints::PicrossHints(const std::vector<std::vector<Block>>& grid) {
    calculate_hints(grid);
}

PicrossHints::PicrossHints(const PicrossHints& other)
    : row_hints{other.row_hints}, col_hints{other.col_hints} {}

PicrossHints& PicrossHints::operator=(const PicrossHints& other) {
    if (this != &other) {
        row_hints = other.row_hints;
        col_hints = other.col_hints;
    }
    return *this;
}

PicrossHints::~PicrossHints() = default;

void PicrossHints::calculate_hints(const std::vector<std::vector<Block>>& grid) {
    size_t size = grid.size();
    row_hints.clear();
    col_hints.clear();

    for (size_t i = 0; i < size; i++) {
        std::vector<int> row_hint;
        int count = 0;

        for (size_t j = 0; j < size; j++) {
            if (grid[i][j].is_correct()) {
                count++;
            } else if (count > 0) {
                row_hint.push_back(count);
                count = 0;
            }
        }

        if (count > 0) {
            row_hint.push_back(count);
        }

        if (row_hint.empty()) {
            row_hint.push_back(0);
        }

        row_hints.push_back(row_hint);
    }

    for (size_t j = 0; j < size; j++) {
        std::vector<int> col_hint;
        int count = 0;

        for (size_t i = 0; i < size; i++) {
            if (grid[i][j].is_correct()) {
                count++;
            } else if (count > 0) {
                col_hint.push_back(count);
                count = 0;
            }
        }

        if (count > 0) {
            col_hint.push_back(count);
        }

        if (col_hint.empty()) {
            col_hint.push_back(0);
        }

        col_hints.push_back(col_hint);
    }
}

const std::vector<std::vector<int>>& PicrossHints::get_row_hints() const { return row_hints; }
const std::vector<std::vector<int>>& PicrossHints::get_col_hints() const { return col_hints; }

size_t PicrossHints::get_max_col_height() const {
    size_t max_height = 0;
    for (const auto& col : col_hints) {
        max_height = std::max(max_height, col.size());
    }
    return max_height;
}

size_t PicrossHints::get_max_row_width() const {
    size_t max_width = 0;
    for (const auto& row : row_hints) {
        max_width = std::max(max_width, row.size());
    }
    return max_width;
}

std::ostream &operator<<(std::ostream &os, const PicrossHints &h) {
    os << "=== HINTS ===\n";
    os << "Linii:\n";
    for (const auto &row: h.row_hints) {
        for (int val: row) os << val << " ";
        os << "\n";
    }
    os << "Coloane:\n";
    for (const auto &col: h.col_hints) {
        for (int val: col) os << val << " ";
        os << "\n";
    }
    return os;
}