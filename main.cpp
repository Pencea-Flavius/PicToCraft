#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

class Block {
    bool corect;
    bool completat;

public:
    explicit Block(bool corect_val) : corect(corect_val), completat(false) {}

    // Constructor de copiere manual
    Block(const Block& other) : corect(other.corect), completat(other.completat) {}

    // Operator= manual
    Block& operator=(const Block& other) {
        if (this != &other) {
            corect = other.corect;
            completat = other.completat;
        }
        return *this;
    }

    // Destructor manual
    ~Block() {}

    void inverseaza() { completat = !completat; }
    [[nodiscard]] bool este_corect() const { return corect; }
    [[nodiscard]] bool este_completat() const { return completat; }

    friend std::ostream& operator<<(std::ostream& os, const Block& b) {
        os << (b.completat ? "#" : ".");
        return os;
    }
};

class IndiciiPicross {
    std::vector<std::vector<int>> indicii_linii;
    std::vector<std::vector<int>> indicii_coloane;

public:
    IndiciiPicross() {}

    explicit IndiciiPicross(const std::vector<std::vector<Block>>& matrice) {
        calculeaza_indicii(matrice);
    }

    // Constructor de copiere manual
    IndiciiPicross(const IndiciiPicross& other)
        : indicii_linii(other.indicii_linii), indicii_coloane(other.indicii_coloane) {}

    // Operator= manual
    IndiciiPicross& operator=(const IndiciiPicross& other) {
        if (this != &other) {
            indicii_linii = other.indicii_linii;
            indicii_coloane = other.indicii_coloane;
        }
        return *this;
    }

    // Destructor manual
    ~IndiciiPicross() {}

    void calculeaza_indicii(const std::vector<std::vector<Block>>& matrice) {
        size_t dim = matrice.size();
        indicii_linii.clear();
        indicii_coloane.clear();

        // Calculează indicii pentru linii
        for (size_t i = 0; i < dim; i++) {
            std::vector<int> indicii_linie;
            int contor = 0;

            for (size_t j = 0; j < dim; j++) {
                if (matrice[i][j].este_corect()) {
                    contor++;
                } else if (contor > 0) {
                    indicii_linie.push_back(contor);
                    contor = 0;
                }
            }

            if (contor > 0) {
                indicii_linie.push_back(contor);
            }

            if (indicii_linie.empty()) {
                indicii_linie.push_back(0);
            }

            indicii_linii.push_back(indicii_linie);
        }

        // Calculează indicii pentru coloane
        for (size_t j = 0; j < dim; j++) {
            std::vector<int> indicii_coloana;
            int contor = 0;

            for (size_t i = 0; i < dim; i++) {
                if (matrice[i][j].este_corect()) {
                    contor++;
                } else if (contor > 0) {
                    indicii_coloana.push_back(contor);
                    contor = 0;
                }
            }

            if (contor > 0) {
                indicii_coloana.push_back(contor);
            }

            if (indicii_coloana.empty()) {
                indicii_coloana.push_back(0);
            }

            indicii_coloane.push_back(indicii_coloana);
        }
    }

    [[nodiscard]] const std::vector<std::vector<int>>& get_indicii_linii() const { return indicii_linii; }
    [[nodiscard]] const std::vector<std::vector<int>>& get_indicii_coloane() const { return indicii_coloane; }

    [[nodiscard]] size_t get_inaltime_maxima_coloane() const {
        size_t max_inaltime = 0;
        for (const auto& coloana : indicii_coloane) {
            max_inaltime = std::max(max_inaltime, coloana.size());
        }
        return max_inaltime;
    }

    [[nodiscard]] size_t get_latime_maxima_linii() const {
        size_t max_latime = 0;
        for (const auto& linie : indicii_linii) {
            max_latime = std::max(max_latime, linie.size());
        }
        return max_latime;
    }

    friend std::ostream& operator<<(std::ostream& os, const IndiciiPicross& ind) {
        os << "=== INDICII PICROSS ===\n";

        os << "Indicii linii:\n";
        for (size_t i = 0; i < ind.indicii_linii.size(); ++i) {
            os << "Linia " << i << ": ";
            for (size_t j = 0; j < ind.indicii_linii[i].size(); ++j) {
                os << ind.indicii_linii[i][j];
                if (j < ind.indicii_linii[i].size() - 1) os << " ";
            }
            os << "\n";
        }

        os << "Indicii coloane:\n";
        for (size_t i = 0; i < ind.indicii_coloane.size(); ++i) {
            os << "Coloana " << i << ": ";
            for (size_t j = 0; j < ind.indicii_coloane[i].size(); ++j) {
                os << ind.indicii_coloane[i][j];
                if (j < ind.indicii_coloane[i].size() - 1) os << " ";
            }
            os << "\n";
        }

        return os;
    }
};

class Grila {
    int dim{};
    std::vector<std::vector<Block>> matrice;
    IndiciiPicross indicii;

public:
    Grila() : dim(0) {}

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
        indicii = IndiciiPicross(matrice);
    }

    // Constructor de copiere manual
    Grila(const Grila& other) : dim(other.dim), matrice(other.matrice), indicii(other.indicii) {}

    // Operator= manual
    Grila& operator=(const Grila& other) {
        if (this != &other) {
            dim = other.dim;
            matrice = other.matrice;
            indicii = other.indicii;
        }
        return *this;
    }

    // Destructor manual
    ~Grila() {}

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
        indicii = IndiciiPicross(matrice);
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

    void afiseaza_indicii() const {
        std::cout << indicii;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grila& g) {
        const auto& indicii_linii = g.indicii.get_indicii_linii();
        const auto& indicii_coloane = g.indicii.get_indicii_coloane();

        size_t inaltime_maxima = g.indicii.get_inaltime_maxima_coloane();
        size_t latime_maxima = g.indicii.get_latime_maxima_linii();

        // Afișează indicii coloanelor
        for (size_t nivel = 0; nivel < inaltime_maxima; nivel++) {
            for (size_t s = 0; s < latime_maxima; s++) {
                os << "  ";
            }
            os << " ";

            for (int j = 0; j < g.dim; j++) {
                const auto& coloana = indicii_coloane[j];
                int index = static_cast<int>(coloana.size()) - static_cast<int>(inaltime_maxima) + static_cast<int>(nivel);

                if (index >= 0) {
                    os << coloana[index] << " ";
                } else {
                    os << "  ";
                }
            }
            os << "\n";
        }

        // Afișează grila cu indicii liniilor
        for (int i = 0; i < g.dim; i++) {
            const auto& linie = indicii_linii[i];

            // Afișează indicii liniei
            for (size_t s = 0; s < latime_maxima - linie.size(); s++) {
                os << "  ";
            }
            for (size_t k = 0; k < linie.size(); k++) {
                os << linie[k];
                if (k < linie.size() - 1) os << " ";
            }

            // Afișează blocurile
            for (int j = 0; j < g.dim; j++) {
                os << " " << g.matrice[i][j];
            }
            os << "\n";
        }

        return os;
    }
};

class Joc {
    Grila grila;

public:
    explicit Joc(const Grila& g) : grila(g) {}

    // Constructor de copiere manual
    Joc(const Joc& other) : grila(other.grila) {}

    // Operator= manual
    Joc& operator=(const Joc& other) {
        if (this != &other) {
            grila = other.grila;
        }
        return *this;
    }

    // Destructor manual
    ~Joc() {}

    void ruleaza() {
        std::cout << "=== PICTOCRAFT ===\n";
        std::cout << "Indicii puzzle-ului:\n";
        grila.afiseaza_indicii();
        std::cout << "\nGrila:\n";
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