//
// Created by zzfla on 11/8/2025.
//

#include "GameManager.h"

#include <ostream>
#include <iostream>

GameManager::GameManager(const Grid& g)
    : grid{g}, start_time{std::chrono::steady_clock::now()} {}

GameManager::GameManager(const GameManager& other)
    : grid{other.grid}, start_time{other.start_time} {}

GameManager& GameManager::operator=(const GameManager& other) {
    if (this != &other) {
        grid = other.grid;
        start_time = other.start_time;
    }
    return *this;
}

GameManager::~GameManager() = default;

long long GameManager::get_elapsed_time() const {
    auto end_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
}

void GameManager::run() {
    while (true) {
        int x, y;
        if (!(std::cin >> x >> y)) {
            break;
        }

        grid.toggle_block(x, y);

        if (grid.is_solved() || grid.is_lost()) {
            break;
        }
    }
}

std::ostream &operator<<(std::ostream &os, const GameManager &g) {
    os << g.grid;
    os << "Timp scurs: " << g.get_elapsed_time() << " secunde\n";
    return os;
}