#include <iostream>

#include "src/GameManager.h"

int main() {
    try {
        GameManager game;
        game.run();
    } catch (const std::exception &e) {
        std::cerr << "Eroare: " << e.what() << "\n";
        return -1;
    }

    return 0;
}