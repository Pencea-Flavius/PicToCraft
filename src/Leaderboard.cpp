#include "Leaderboard.h"
#include "Exceptions.h"
#include <algorithm>
#include <fstream>
#include <filesystem>

Leaderboard::Leaderboard() = default;

void Leaderboard::load(const std::string& filename) {
    entries.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw AssetLoadException(filename, "Leaderboard File");
    }

    std::string name;
    int score;
    while (file >> name >> score) {
        entries.push_back({name, score});
    }

    // Sort just in case file was tampered
    std::ranges::sort(entries, [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    if (entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }
}

void Leaderboard::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw AssetLoadException(filename, "Leaderboard File");
    }

    for (const auto& entry : entries) {
        file << entry.name << " " << entry.score << "\n";
    }
}

bool Leaderboard::addEntry(const std::string& name, int score) {
    entries.push_back({name, score});
    
    // Sort descending by score
    std::ranges::sort(entries, [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    bool madeIt = false;
    // Check if within top MAX_ENTRIES
    for (size_t i = 0; i < entries.size() && i < MAX_ENTRIES; ++i) {
        if (entries[i].name == name && entries[i].score == score) {
            madeIt = true;
            break;
        }
    }

    if (entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }

    return madeIt;
}

const std::vector<ScoreEntry>& Leaderboard::getEntries() const {
    return entries;
}
