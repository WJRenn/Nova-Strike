#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Leaderboard {
public:
    struct Entry {
        std::string name;
        int score = 0;
    };

    explicit Leaderboard(std::filesystem::path filePath);

    void reload();
    void addScore(const std::string& name, int score);

    const std::vector<Entry>& getEntries() const noexcept { return entries; }

private:
    static constexpr std::size_t MAX_ENTRIES = 10;

    void sortAndTrim();
    void save() const;

    std::filesystem::path filePath;
    std::vector<Entry> entries;
};
