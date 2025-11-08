//
// Created by zzfla on 11/8/2025.
//

#include "Block.h"
#include <ostream>

Block::Block(bool correct_val) : correct{correct_val}, completed{false} {}

Block::Block(const Block& other) : correct{other.correct}, completed{other.completed} {}

Block& Block::operator=(const Block& other) {
    if (this != &other) {
        correct = other.correct;
        completed = other.completed;
    }
    return *this;
}

Block::~Block() = default;

void Block::toggle() { completed = !completed; }

bool Block::is_correct() const { return correct; }

bool Block::is_completed() const { return completed; }

std::ostream& operator<<(std::ostream& os, const Block& b) {
    os << (b.completed ? "#" : ".");
    return os;
}