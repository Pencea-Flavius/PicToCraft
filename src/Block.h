//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_BLOCK_H
#define OOP_BLOCK_H

#include <iosfwd>

class Block {
    bool correct;
    bool completed;

public:
    explicit Block(bool correct_val);
    Block(const Block& other);
    Block& operator=(const Block& other);
    ~Block();

    void toggle();
    [[nodiscard]] bool is_correct() const;
    [[nodiscard]] bool is_completed() const;

    friend std::ostream& operator<<(std::ostream& os, const Block& b);
};

#endif //OOP_BLOCK_H