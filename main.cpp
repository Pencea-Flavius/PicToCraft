#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>

class Block {
    bool corect;
    bool completat;

public:
    explicit Block(bool corect_val) : corect(corect_val), completat(false) {
    }

    void inverseaza() { completat = !completat; }
    [[nodiscard]] bool este_corect() const { return corect; }
    [[nodiscard]] bool este_completat() const { return completat; }

    friend std::ostream &operator<<(std::ostream &os, const Block &b) {
        os << (b.completat ? "#" : ".");
        return os;
    }
};

class Grila {
    int dim{};
    std::vector<std::vector<Block> > matrice;

public:
    Grila() = default;

    void citeste_din_fisier(const std::string &nume_fisier) {
        std::ifstream fin(nume_fisier);
        if (!fin) {
            std::cout << "Eroare la deschiderea fisierului " << nume_fisier << "\n";
            return;
        }

        fin >> dim;
        matrice.clear();
        matrice.resize(dim); // doar rânduri pentru moment

        std::string linie;
        for (int i = 0; i < dim; i++) {
            fin >> linie;
            matrice[i].clear();
            for (int j = 0; j < dim && j < static_cast<int>(linie.size()); j++) {
                matrice[i].emplace_back(linie[j] == '1'); // corect sau nu
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

    void afiseaza() const {
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++)
                std::cout << matrice[i][j];
            std::cout << "\n";
        }
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
};

class Joc {
    Grila grila;

public:
    explicit Joc(Grila g) : grila(std::move(g)) {
    }

    void ruleaza() {
        std::cout << "=== PICTOCRAFT ===\n";
        grila.afiseaza();

        while (true) {
            int x, y;
            std::cout << "\nIntrodu coordonatele x y sau -1 -1 ca sa iesi: ";
            std::cin >> x >> y;
            if (x == -1 && y == -1) break;

            grila.toggle_bloc(x, y);
            grila.afiseaza();

            if (grila.este_castigata()) {
                std::cout << "\n Bravo, ai castigat!\n";
                break;
            }
        }
    }
};

int main() {
    Grila g;
    g.citeste_din_fisier("item.txt");

    Joc joc(g);
    joc.ruleaza();

    return 0;
}
