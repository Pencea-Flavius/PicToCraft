//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_PICROSSHINTS_H
#define OOP_PICROSSHINTS_H
#include <iosfwd>
#include <vector>

#include "Block.h"

class PicrossHints {
    std::vector<std::vector<int>> row_hints;
    std::vector<std::vector<int>> col_hints;

public:
    PicrossHints();
    explicit PicrossHints(const std::vector<std::vector<Block>>& grid);
    PicrossHints(const PicrossHints& other);
    PicrossHints& operator=(const PicrossHints& other);
    ~PicrossHints();

    void calculate_hints(const std::vector<std::vector<Block>>& grid);

    [[nodiscard]] const std::vector<std::vector<int>>& get_row_hints() const;
    [[nodiscard]] const std::vector<std::vector<int>>& get_col_hints() const;

    [[nodiscard]] size_t get_max_col_height() const;
    [[nodiscard]] size_t get_max_row_width() const;

    friend std::ostream &operator<<(std::ostream &os, const PicrossHints &h);
};

#endif //OOP_PICROSSHINTS_H