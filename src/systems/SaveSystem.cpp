#include "systems/SaveSystem.hpp"

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <system_error>
#include <utility>

namespace {
constexpr std::array<char, 4> SAVE_MAGIC{'S', 'S', 'G', 'M'};
constexpr std::uint32_t SAVE_VERSION = 1;
constexpr std::uint32_t MAX_ENEMY_COUNT = 100;

template<typename T>
bool writeValue(std::ostream& stream, const T& value) {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return static_cast<bool>(stream);
}

template<typename T>
bool readValue(std::istream& stream, T& value) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(T));
    return static_cast<bool>(stream);
}

bool isValidEnemyType(EnemyType type) {
    switch (type) {
        case EnemyType::Alpha:
        case EnemyType::Beta:
        case EnemyType::Gamma:
        case EnemyType::Dragon:
        case EnemyType::Monster:
            return true;
        default:
            return false;
    }
}

int getMaximumEnemyHealth(EnemyType type) {
    switch (type) {
        case EnemyType::Alpha: return 3;
        case EnemyType::Beta: return 5;
        case EnemyType::Gamma: return 7;
        case EnemyType::Dragon:
        case EnemyType::Monster: return 100;
        default: return 0;
    }
}

bool isValidPosition(float x, float y) {
    return std::isfinite(x) && std::isfinite(y)
        && x >= -500.f && x <= 1500.f
        && y >= -500.f && y <= 1500.f;
}
}

SaveSystem::SaveSystem(std::filesystem::path path, int maximumHealth)
    : savePath(std::move(path))
    , temporaryPath(savePath.parent_path() / "savegame.tmp")
    , maximumPlayerHealth(maximumHealth) {
}

bool SaveSystem::exists() const {
    std::error_code error;
    return std::filesystem::exists(savePath, error) && !error;
}

bool SaveSystem::save(const SavedGame& data) const {
    if (!validate(data)) {
        std::cerr << "Refusing to save invalid game data\n";
        return false;
    }

    std::ofstream file(temporaryPath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    file.write(SAVE_MAGIC.data(), SAVE_MAGIC.size());
    const std::uint32_t enemyCount = static_cast<std::uint32_t>(data.enemies.size());
    bool success = writeValue(file, SAVE_VERSION)
        && writeValue(file, data.level)
        && writeValue(file, data.score)
        && writeValue(file, data.health)
        && writeValue(file, data.shipType)
        && writeValue(file, data.mouseControl)
        && writeValue(file, data.playerX)
        && writeValue(file, data.playerY)
        && writeValue(file, enemyCount);

    for (const auto& enemy : data.enemies) {
        success = success
            && writeValue(file, enemy.type)
            && writeValue(file, enemy.x)
            && writeValue(file, enemy.y)
            && writeValue(file, enemy.health);
    }

    file.flush();
    success = success && static_cast<bool>(file);
    file.close();
    if (!success) {
        std::filesystem::remove(temporaryPath);
        std::cerr << "Failed to save game\n";
        return false;
    }

    std::error_code error;
    std::filesystem::remove(savePath, error);
    error.clear();
    std::filesystem::rename(temporaryPath, savePath, error);
    if (error) {
        std::filesystem::remove(temporaryPath);
        std::cerr << "Failed to replace save file\n";
        return false;
    }

    return true;
}

std::optional<SavedGame> SaveSystem::load() const {
    std::ifstream file(savePath, std::ios::binary);
    if (!file.is_open()) return std::nullopt;

    std::array<char, 4> magic{};
    file.read(magic.data(), magic.size());
    if (!file || magic != SAVE_MAGIC) {
        std::cerr << "Invalid save file header\n";
        return std::nullopt;
    }

    std::uint32_t version{};
    std::uint32_t enemyCount{};
    SavedGame data;
    if (!readValue(file, version)
        || version != SAVE_VERSION
        || !readValue(file, data.level)
        || !readValue(file, data.score)
        || !readValue(file, data.health)
        || !readValue(file, data.shipType)
        || !readValue(file, data.mouseControl)
        || !readValue(file, data.playerX)
        || !readValue(file, data.playerY)
        || !readValue(file, enemyCount)) {
        return std::nullopt;
    }

    if (enemyCount == 0 || enemyCount > MAX_ENEMY_COUNT) {
        std::cerr << "Invalid enemy count\n";
        return std::nullopt;
    }

    data.enemies.reserve(enemyCount);
    for (std::uint32_t i = 0; i < enemyCount; ++i) {
        SavedEnemy enemy;
        if (!readValue(file, enemy.type)
            || !readValue(file, enemy.x)
            || !readValue(file, enemy.y)
            || !readValue(file, enemy.health)) {
            std::cerr << "Incomplete enemy data\n";
            return std::nullopt;
        }
        data.enemies.push_back(enemy);
    }

    if (!validate(data)) {
        std::cerr << "Invalid save data\n";
        return std::nullopt;
    }
    return data;
}

bool SaveSystem::deleteSave() const {
    std::error_code error;
    const bool removed = std::filesystem::remove(savePath, error);
    if (error) {
        std::cerr << "Failed to delete save: " << error.message() << '\n';
        return false;
    }
    return removed;
}

bool SaveSystem::validate(const SavedGame& data) const {
    if (data.level < 1 || data.level > 5
        || data.score < 0
        || data.health < 1 || data.health > maximumPlayerHealth
        || data.shipType < 1 || data.shipType > 3
        || data.mouseControl > 1
        || !isValidPosition(data.playerX, data.playerY)
        || data.enemies.empty() || data.enemies.size() > MAX_ENEMY_COUNT) {
        return false;
    }

    for (const auto& enemy : data.enemies) {
        if (!isValidEnemyType(enemy.type)
            || !isValidPosition(enemy.x, enemy.y)
            || enemy.health < 1
            || enemy.health > getMaximumEnemyHealth(enemy.type)) {
            return false;
        }
    }
    return true;
}
