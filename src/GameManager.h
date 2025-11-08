//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <chrono>
#include <iosfwd>

#include "Grid.h"

class GameManager {
    Grid grid;
    std::chrono::steady_clock::time_point start_time;

public:
    explicit GameManager(const Grid& g);
    GameManager(const GameManager& other);
    GameManager& operator=(const GameManager& other);
    ~GameManager();

    [[nodiscard]] long long get_elapsed_time() const;
    void run();

    friend std::ostream &operator<<(std::ostream &os, const GameManager &g);
};

#endif //OOP_GAMEMANAGER_H