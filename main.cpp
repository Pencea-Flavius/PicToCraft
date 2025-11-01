#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

class Block {
    bool correct;
    bool completed;

public:
    explicit Block(bool correct_val) : correct{correct_val}, completed{false} {}

    Block(const Block& other) : correct{other.correct}, completed{other.completed} {}

    Block& operator=(const Block& other) {
        if (this != &other) {
            correct = other.correct;
            completed = other.completed;
        }
        return *this;
    }

    ~Block() {}

    void toggle() { completed = !completed; }
    [[nodiscard]] bool is_correct() const { return correct; }
    [[nodiscard]] bool is_completed() const { return completed; }

    friend std::ostream& operator<<(std::ostream& os, const Block& b) {
        os << (b.completed ? "#" : ".");
        return os;
    }
};

class PicrossHints {
    std::vector<std::vector<int>> row_hints;
    std::vector<std::vector<int>> col_hints;

public:
    PicrossHints() : row_hints{}, col_hints{} {}

    explicit PicrossHints(const std::vector<std::vector<Block>>& grid) {
        calculate_hints(grid);
    }

    PicrossHints(const PicrossHints& other)
        : row_hints{other.row_hints}, col_hints{other.col_hints} {}

    PicrossHints& operator=(const PicrossHints& other) {
        if (this != &other) {
            row_hints = other.row_hints;
            col_hints = other.col_hints;
        }
        return *this;
    }

    ~PicrossHints() {}

    void calculate_hints(const std::vector<std::vector<Block>>& grid) {
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

    [[nodiscard]] const std::vector<std::vector<int>>& get_row_hints() const { return row_hints; }
    [[nodiscard]] const std::vector<std::vector<int>>& get_col_hints() const { return col_hints; }

    [[nodiscard]] size_t get_max_col_height() const {
        size_t max_height = 0;
        for (const auto& col : col_hints) {
            max_height = std::max(max_height, col.size());
        }
        return max_height;
    }

    [[nodiscard]] size_t get_max_row_width() const {
        size_t max_width = 0;
        for (const auto& row : row_hints) {
            max_width = std::max(max_width, row.size());
        }
        return max_width;
    }
    friend std::ostream& operator<<(std::ostream& os, const PicrossHints& h) {
        os << "=== HINTS ===\n";
        os << "Linii:\n";
        for (const auto& row : h.row_hints) {
            for (int val : row) os << val << " ";
            os << "\n";
        }
        os << "Coloane:\n";
        for (const auto& col : h.col_hints) {
            for (int val : col) os << val << " ";
            os << "\n";
        }
        return os;
    }
};

class Grid {
    int size;
    std::vector<std::vector<Block>> blocks;
    int total_correct_blocks;
    int completed_blocks;               // number of blocks currently marked as completed
    int correct_completed_blocks;       // number of completed blocks that are actually correct
    PicrossHints hints;
    int mistakes;
    int score;
    bool score_mode;

public:
    Grid() : size{}, blocks{}, total_correct_blocks{}, completed_blocks{}, correct_completed_blocks{}, hints{}, mistakes{}, score{}, score_mode{false} {}

    Grid(int grid_size, const std::vector<std::vector<bool>>& pattern, bool use_score_mode = false)
        : size{grid_size}, total_correct_blocks{0}, completed_blocks{0}, correct_completed_blocks{0}, mistakes{0}, score{1000}, score_mode{use_score_mode} {
        blocks.resize(size);
        for (int i = 0; i < size; i++) {
            blocks[i].reserve(size);
            for (int j = 0; j < size; j++) {
                bool val = (i < static_cast<int>(pattern.size()) &&
                           j < static_cast<int>(pattern[i].size())) ? pattern[i][j] : false;
                if (val) total_correct_blocks++;
                blocks[i].emplace_back(val);
            }
        }
        hints = PicrossHints(blocks);
    }

    Grid(const Grid& other) : size(other.size), blocks(other.blocks), total_correct_blocks(other.total_correct_blocks), completed_blocks(other.completed_blocks), correct_completed_blocks(other.correct_completed_blocks), hints(other.hints),
                             mistakes(other.mistakes), score(other.score), score_mode(other.score_mode) {}

    Grid& operator=(const Grid& other) {
        if (this != &other) {
            size = other.size;
            blocks = other.blocks;
            total_correct_blocks = other.total_correct_blocks;
            completed_blocks = other.completed_blocks;
            correct_completed_blocks = other.correct_completed_blocks;
            hints = other.hints;
            mistakes = other.mistakes;
            score = other.score;
            score_mode = other.score_mode;
        }
        return *this;
    }

    ~Grid() {}

    void load_from_file(const std::string& filename, bool use_score_mode = false) {
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Eroare la deschiderea fisierului " << filename << "\n";
            return;
        }

        file >> size;
        blocks.clear();
        blocks.resize(size);
        total_correct_blocks = 0;
        completed_blocks = 0;
        correct_completed_blocks = 0;

        std::string line;
        for (int i = 0; i < size; i++) {
            file >> line;
            blocks[i].clear();
            for (int j = 0; j < size; j++) {
                bool val = (j < static_cast<int>(line.size()) && line[j] == '1');
                if (val) total_correct_blocks++;
                blocks[i].emplace_back(val);
            }
        }
        file.close();
        hints = PicrossHints(blocks);
        mistakes = 0;
        score = 1000;
        score_mode = use_score_mode;
        std::cout << "Grila incarcata (dim = " << size << ")\n";
    }

    void generate_random(int grid_size, bool use_score_mode = false, double density = 0.5) {
        size = grid_size;
        blocks.clear();
        blocks.resize(size);
        total_correct_blocks = 0;
        completed_blocks = 0;
        correct_completed_blocks = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                bool val = dis(gen) < density;
                if (val) total_correct_blocks++;
                blocks[i].emplace_back(val);
            }
        }

        hints = PicrossHints(blocks);
        mistakes = 0;
        score = 1000;
        score_mode = use_score_mode;
        std::cout << "Grila random generata (dim = " << size << ")\n";
    }

    void toggle_block(int x, int y) {
        if (x < 0 || y < 0 || x >= size || y >= size) {
            std::cout << "Coordonate invalide!\n";
            return;
        }

        bool was_completed = blocks[x][y].is_completed();
        bool block_correct = blocks[x][y].is_correct();


        blocks[x][y].toggle();

        bool now_completed = blocks[x][y].is_completed();

        if (!was_completed && now_completed) {
            completed_blocks++;
            if (block_correct) correct_completed_blocks++;
        } else if (was_completed && !now_completed) {
            completed_blocks--;
            if (block_correct) correct_completed_blocks--;
        }

        if (score_mode) {
            if (now_completed && block_correct) {
                score += 200;
                std::cout << "Corect! +200 puncte\n";
            } else if (now_completed && !block_correct) {
                score -= 100;
                std::cout << "Gresit! -100 puncte\n";
            } else if (block_correct) {
                score -= 300;
                std::cout << "Gresit! -300 puncte\n";
            }
            if (score < 0) score = 0;
        } else {
            if ((now_completed && !block_correct) || (!now_completed && block_correct)) {
                mistakes++;
                std::cout << "Gresit! Eroare " << mistakes << "/3\n";
            }
        }
    }

    [[nodiscard]] bool is_solved() const {
        // Update to only make 2 comparisons, not checking the hole matrix every time
        return completed_blocks == total_correct_blocks &&
               correct_completed_blocks == total_correct_blocks;
    }

    [[nodiscard]] bool is_lost() const {
        return !score_mode && mistakes >= 3;
    }

    [[nodiscard]] int get_score() const { return score; }
    [[nodiscard]] int get_mistakes() const { return mistakes; }
    [[nodiscard]] bool get_score_mode() const { return score_mode; }
    [[nodiscard]] int get_size() const { return size; }

    [[nodiscard]] std::string to_string() const {
        std::string output;

        if (score_mode) {
            output += "Scor: " + std::to_string(score) + "\n";
        } else {
            output += "Greseli: " + std::to_string(mistakes) + "/3\n";
        }

        const auto& row_hints = hints.get_row_hints();
        const auto& col_hints = hints.get_col_hints();

        size_t max_col_height = hints.get_max_col_height();
        size_t max_row_width = hints.get_max_row_width();

        for (size_t level = 0; level < max_col_height; level++) {
            for (size_t s = 0; s < max_row_width; s++) {
                output += "  ";
            }
            output += " ";

            for (int j = 0; j < size; j++) {
                const auto& col = col_hints[j];
                int index = static_cast<int>(col.size()) - static_cast<int>(max_col_height) + static_cast<int>(level);

                if (index >= 0) {
                    output += std::to_string(col[index]) + " ";
                } else {
                    output += "  ";
                }
            }
            output += "\n";
        }

        for (int i = 0; i < size; i++) {
            const auto& row = row_hints[i];
            for (size_t s = 0; s < max_row_width - row.size(); s++) {
                output += "  ";
            }
            for (size_t k = 0; k < row.size(); k++) {
                output += std::to_string(row[k]);
                if (k < row.size() - 1) output += " ";
            }

            for (int j = 0; j < size; j++) {
                output += " ";
                output += (blocks[i][j].is_completed() ? "#" : ".");
            }
            output += "\n";
        }

        return output;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid& g) {
        os << g.to_string();
        return os;
    }
};

class Game {
    Grid grid;
    std::chrono::steady_clock::time_point start_time;

public:
    explicit Game(const Grid& g) : grid{g}, start_time{std::chrono::steady_clock::now()} {}

    Game(const Game& other) : grid{other.grid}, start_time{other.start_time} {}

    Game& operator=(const Game& other) {
        if (this != &other) {
            grid = other.grid;
            start_time = other.start_time;
        }
        return *this;
    }

    ~Game() {}

    [[nodiscard]] long long get_elapsed_time() const {
        auto end_time = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    }

    void run() {
        std::string output = "=== PICTOCRAFT ===\n";
        output += grid.get_score_mode() ? "Mod: SCOR - Incepi cu 1000 puncte\n" : "Mod: GRESELI - Ai voie 3 greseli\n";
        output += "Dimensiune: " + std::to_string(grid.get_size()) + "x" + std::to_string(grid.get_size()) + "\n";
        output += grid.to_string();
        std::cout << output;

        while (true) {
            int x, y;
            std::cout << "\nIntrodu coordonatele x y: ";
            if (!(std::cin >> x >> y)) {
                std::cout << "Input invalid, ies din joc.\n";
                break;
            }

            grid.toggle_block(x, y);

            std::string state = "\n";
            if (grid.get_score_mode()) {
                state += "Scor: " + std::to_string(grid.get_score()) + " | ";
            } else {
                state += "Greseli: " + std::to_string(grid.get_mistakes()) + "/3 | ";
            }
            state += "Timp: " + std::to_string(get_elapsed_time()) + "s\n";
            state += grid.to_string();
            std::cout << state;

            if (grid.is_solved()) {
                std::string final_msg = "\n=== BRAVO AI CASTIGAT! ===\n";
                if (grid.get_score_mode()) {
                    final_msg += "Scor final: " + std::to_string(grid.get_score()) + "\n";
                }
                final_msg += "Timp total: " + std::to_string(get_elapsed_time()) + " secunde\n";
                std::cout << final_msg;
                break;
            }

            if (grid.is_lost()) {
                std::cout << "\n=== GAME OVER! Ai facut 3 greseli. ===\n";
                break;
            }
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const Game& g) {
        os << "=== STARE JOC ===\n";
        os << g.grid;
        os << "Timp scurs: " << g.get_elapsed_time() << " secunde\n";
        return os;
    }
};

int main() {
    auto start_main = std::chrono::steady_clock::now();

    std::string menu = "=== PICTOCRAFT ===\n1. Joc din fisier\n2. Joc random\nAlege optiunea (1 sau 2): ";
    std::cout << menu;

    int option;
    std::cin >> option;

    if (option == 1 || option == 2) {
        std::string mode_menu = "\nAlege modul de joc:\n1. Mod scor (punctaj)\n2. Mod greseli (3 greseli = game over)\nAlege modul (1 sau 2): ";
        std::cout << mode_menu;

        int mode_choice;
        std::cin >> mode_choice;
        bool score_mode = (mode_choice == 1);

        Grid grid;

        if (option == 1) {
            std::string file_menu = "\nAlege fisierul:\n1. item.txt\n2. mob.txt\nAlege fisierul (1 sau 2): ";
            std::cout << file_menu;

            int file_choice;
            std::cin >> file_choice;

            if (file_choice == 1) {
                grid.load_from_file("item.txt", score_mode);
            } else if (file_choice == 2) {
                grid.load_from_file("mob.txt", score_mode);
            } else {
                std::cout << "Optiune invalida. Iesire.\n";
                return 0;
            }
        } else {
            int size;
            std::cout << "Introdu dimensiunea grilei (5-15): ";
            std::cin >> size;
            size = std::max(5, std::min(15, size));
            grid.generate_random(size, score_mode);
        }

        Game game(grid);
        game.run();
    }

    auto end_main = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_main - start_main);
    std::cout << "\nTimp total in program: " << duration.count() << " secunde\n";

    return 0;

}