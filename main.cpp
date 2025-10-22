#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Block {
    bool corect;
    bool completat;

public:
    explicit Block(bool corect_val) : corect(corect_val), completat(false) {}
    Block(const Block&) = default;
    Block& operator=(const Block&) = default;
    ~Block() = default;

    void inverseaza() { completat = !completat; }
    [[nodiscard]] bool este_corect() const { return corect; }
    [[nodiscard]] bool este_completat() const { return completat; }

    friend std::ostream& operator<<(std::ostream& os, const Block& b) {
        os << (b.completat ? "#" : ".");
        return os;
    }
};

class Grila {
    int dim{};
    std::vector<std::vector<Block>> matrice;

public:
    Grila() = default;

    // Constructor cu parametri
    Grila(int dimensiune, const std::vector<std::vector<bool>>& pattern) : dim(dimensiune) {
        matrice.resize(dim);
        for (int i = 0; i < dim; i++) {
            matrice[i].reserve(dim);
            for (int j = 0; j < dim; j++) {
                bool val = (i < static_cast<int>(pattern.size()) &&
                           j < static_cast<int>(pattern[i].size())) ? pattern[i][j] : false;
                matrice[i].emplace_back(val);
            }
        }
    }

    Grila(const Grila&) = default;
    Grila& operator=(const Grila&) = default;
    ~Grila() = default;

    void citeste_din_fisier(const std::string& nume_fisier) {
        std::ifstream fin(nume_fisier);
        if (!fin) {
            std::cout << "Eroare la deschiderea fisierului " << nume_fisier << "\n";
            return;
        }

        fin >> dim;
        matrice.clear();
        matrice.resize(dim);

        std::string linie;
        for (int i = 0; i < dim; i++) {
            fin >> linie;
            matrice[i].clear();
            for (int j = 0; j < dim; j++) {
                bool val = (j < static_cast<int>(linie.size()) && linie[j] == '1');
                matrice[i].emplace_back(val);
            }
        }
        fin.close();
        std::cout << "Grila incarcata (dim = " << dim << ")\n";
    }

    void toggle_bloc(int x, int y) {
        if (x < 0 || y < 0 || x >= dim || y >= dim) {
            std::cout << "Coordonate invalide!\n";
            return;
        }
        matrice[x][y].inverseaza();
    }

    [[nodiscard]] bool este_castigata() const {
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (matrice[i][j].este_corect() && !matrice[i][j].este_completat())
                    return false;
                if (!matrice[i][j].este_corect() && matrice[i][j].este_completat())
                    return false;
            }
        }
        return true;
    }


    friend std::ostream& operator<<(std::ostream& os, const Grila& g) {
        os << "Grila " << g.dim << "x" << g.dim << ":\n";
        for (int i = 0; i < g.dim; i++) {
            for (int j = 0; j < g.dim; j++)
                os << g.matrice[i][j];
            os << "\n";
        }
        return os;
    }
};

class Joc {
    Grila grila;

public:
    explicit Joc(const Grila& g) : grila(g) {}
    Joc(const Joc&) = default;
    Joc& operator=(const Joc&) = default;
    ~Joc() = default;

    void ruleaza() {
        std::cout << "=== PICTOCRAFT ===\n";
        std::cout << grila;

        while (true) {
            int x, y;
            std::cout << "\nIntrodu coordonatele x y sau -1 -1 ca sa iesi: ";
            if (!(std::cin >> x >> y)) {
                std::cout << "Input invalid sau EOF, ies din joc.\n";
                break;
            }
            if (x == -1 && y == -1) break;

            grila.toggle_bloc(x, y);
            std::cout << grila;

            if (grila.este_castigata()) {
                std::cout << "\nBravo, ai castigat!\n";
                break;
            }
        }
    }


    friend std::ostream& operator<<(std::ostream& os, const Joc& j) {
        os << "=== STARE JOC ===\n";
        os << j.grila;
        os << "Status: " << (j.grila.este_castigata() ? "CASTIGAT" : "IN DESFASURARE") << "\n";
        return os;
    }
};

int main() {
    Grila g;
    g.citeste_din_fisier("item.txt");

    Joc joc(g);
    joc.ruleaza();

    return 0;
}