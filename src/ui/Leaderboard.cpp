#include "ui/Leaderboard.hpp"

#include <algorithm>
#include <fstream>
#include <utility>

Leaderboard::Leaderboard(std::filesystem::path path)
    : filePath(std::move(path)) {
    reload();
}

void Leaderboard::reload() {
    entries.clear();

    std::ifstream file(filePath);
    std::string name;
    int score = 0;
    while (file >> name >> score) {
        entries.push_back({std::move(name), score});
    }

    sortAndTrim();
}

void Leaderboard::addScore(const std::string& name, int score) {
    entries.push_back({name, score});
    sortAndTrim();
    save();
}

void Leaderboard::sortAndTrim() {
    std::sort(entries.begin(), entries.end(),
        [](const Entry& lhs, const Entry& rhs) {
            return lhs.score > rhs.score;
        });

    if (entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }
}

void Leaderboard::save() const {
    std::ofstream file(filePath);
    if (!file) return;

    for (const auto& entry : entries) {
        file << entry.name << ' ' << entry.score << '\n';
    }
}
