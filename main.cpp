#include <iostream>
#include <SFML/Graphics.hpp>
#include "src/MenuResolution.h"
#include <fstream>
#include <algorithm>
#include "src/GameManager.h"
#include "src/Grid.h"

int main() {
    int option;
    std::cin >> option;

    if (option == 1 || option == 2) {
        int mode_choice;
        std::cin >> mode_choice;
        bool score_mode = (mode_choice == 1);

        Grid grid;

        if (option == 1) {
            int file_choice;
            std::cin >> file_choice;
            if (file_choice == 1) {
                grid.load_from_file("item.txt", score_mode);
            } else if (file_choice == 2) {
                grid.load_from_file("mob.txt", score_mode);
            } else {
                return 0;
            }
        } else {
            int size;
            std::cin >> size;
            size = std::max(5, std::min(15, size));
            grid.generate_random(size, score_mode);
        }

        GameManager game(grid);
        game.run();
    }

    return 0;
}
