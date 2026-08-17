#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

#include "entities/enemies/EnemyType.hpp"

struct SavedEnemy {
    EnemyType type{};
    float x{};
    float y{};
    std::int32_t health{};
};

struct SavedGame {
    std::uint32_t level{};
    std::int32_t score{};
    std::int32_t health{};
    std::uint8_t shipType{};
    std::uint8_t mouseControl{};
    float playerX{};
    float playerY{};
    std::vector<SavedEnemy> enemies;
};

class SaveSystem {
public:
    SaveSystem(std::filesystem::path savePath, int maximumPlayerHealth);

    [[nodiscard]] bool exists() const;
    [[nodiscard]] bool save(const SavedGame& data) const;
    [[nodiscard]] std::optional<SavedGame> load() const;
    bool deleteSave() const;

private:
    [[nodiscard]] bool validate(const SavedGame& data) const;

    std::filesystem::path savePath;
    std::filesystem::path temporaryPath;
    int maximumPlayerHealth;
};
