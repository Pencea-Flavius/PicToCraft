//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_PICROSSHINTS_H
#define OOP_PICROSSHINTS_H
#include <iosfwd>
#include <vector>

#include "Block.h"

class PicrossHints {
    std::vector<std::vector<int> > row_hints;
    std::vector<std::vector<int> > col_hints;
    std::vector<std::vector<int> > row_hints_webbed;
    std::vector<std::vector<int> > col_hints_webbed;

public:
    PicrossHints() = default;

    explicit PicrossHints(const std::vector<std::vector<bool> > &grid);

    [[nodiscard]] const std::vector<std::vector<int> > &get_row_hints() const {
        return row_hints;
    }

    [[nodiscard]] const std::vector<std::vector<int> > &get_col_hints() const {
        return col_hints;
    }

    [[nodiscard]] size_t get_max_row_width() const;

    [[nodiscard]] size_t get_max_col_height() const;

    void setWebHealth(bool isRow, int line, int index, int health);

    [[nodiscard]] bool isWebbed(bool isRow, int line, int index) const;

    [[nodiscard]] int getWebHealth(bool isRow, int line, int index) const;

    friend std::ostream &operator<<(std::ostream &os, const PicrossHints &hints);
};

#endif // OOP_PICROSSHINTS_H