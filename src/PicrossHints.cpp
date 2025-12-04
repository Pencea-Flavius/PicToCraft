#include "PicrossHints.h"
#include <algorithm>
#include <iostream>

PicrossHints::PicrossHints(const std::vector<std::vector<bool> > &grid) {
    if (grid.empty())
        return;

    // Calculate row hints
    for (const auto &row: grid) {
        std::vector<int> current_row_hints;
        int count = 0;
        for (bool cell: row) {
            if (cell) {
                count++;
            } else if (count > 0) {
                current_row_hints.push_back(count);
                count = 0;
            }
        }
        if (count > 0) {
            current_row_hints.push_back(count);
        }
        if (current_row_hints.empty()) {
            current_row_hints.push_back(0);
        }
        row_hints.push_back(current_row_hints);
    }

    // Calculate col hints
    size_t cols = grid[0].size();
    for (size_t j = 0; j < cols; ++j) {
        std::vector<int> current_col_hints;
        int count = 0;
        for (const auto & i : grid) {
            if (i[j]) {
                count++;
            } else if (count > 0) {
                current_col_hints.push_back(count);
                count = 0;
            }
        }
        if (count > 0) {
            current_col_hints.push_back(count);
        }
        if (current_col_hints.empty()) {
            current_col_hints.push_back(0);
        }
        col_hints.push_back(current_col_hints);
    }

    // Initialize webbed state (0 health = not webbed)
    row_hints_webbed.resize(row_hints.size());
    for (size_t i = 0; i < row_hints.size(); ++i) {
        row_hints_webbed[i].resize(row_hints[i].size(), 0);
    }

    col_hints_webbed.resize(col_hints.size());
    for (size_t i = 0; i < col_hints.size(); ++i) {
        col_hints_webbed[i].resize(col_hints[i].size(), 0);
    }
}

size_t PicrossHints::get_max_row_width() const {
    size_t max_width = 0;
    for (const auto &row: row_hints) {
        max_width = std::max(max_width, row.size());
    }
    return max_width;
}

size_t PicrossHints::get_max_col_height() const {
    size_t max_height = 0;
    for (const auto &col: col_hints) {
        max_height = std::max(max_height, col.size());
    }
    return max_height;
}

void PicrossHints::setWebHealth(bool isRow, int line, int index, int health) {
    if (isRow) {
        if (line >= 0 && line < row_hints_webbed.size() && index >= 0 &&
            index < row_hints_webbed[line].size()) {
            row_hints_webbed[line][index] = health;
        }
    } else {
        if (line >= 0 && line < col_hints_webbed.size() && index >= 0 &&
            index < col_hints_webbed[line].size()) {
            col_hints_webbed[line][index] = health;
        }
    }
}

bool PicrossHints::isWebbed(bool isRow, int line, int index) const {
    return getWebHealth(isRow, line, index) > 0;
}

int PicrossHints::getWebHealth(bool isRow, int line, int index) const {
    if (isRow) {
        if (line >= 0 && line < row_hints_webbed.size() && index >= 0 &&
            index < row_hints_webbed[line].size()) {
            return row_hints_webbed[line][index];
        }
    } else {
        if (line >= 0 && line < col_hints_webbed.size() && index >= 0 &&
            index < col_hints_webbed[line].size()) {
            return col_hints_webbed[line][index];
        }
    }
    return 0;
}

std::ostream &operator<<(std::ostream &os, const PicrossHints &hints) {
    os << "Row Hints:\n";
    for (const auto &row: hints.row_hints) {
        for (int val: row) {
            os << val << " ";
        }
        os << "\n";
    }
    os << "Col Hints:\n";
    for (const auto &col: hints.col_hints) {
        for (int val: col) {
            os << val << " ";
        }
        os << "\n";
    }
    return os;
}