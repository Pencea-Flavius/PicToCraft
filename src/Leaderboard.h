#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>

struct ScoreEntry {
    std::string name;
    int score;
};

class Leaderboard {
public:
    Leaderboard();
    
    void load(const std::string& filename);
    void save(const std::string& filename) const;
    
    // Returns true if the score made it into the top 5
    bool addEntry(const std::string& name, int score);
    
    const std::vector<ScoreEntry>& getEntries() const;

private:
    std::vector<ScoreEntry> entries;
    static const int MAX_ENTRIES = 5;
};

#endif // LEADERBOARD_H
