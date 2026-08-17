#pragma once

#include <cstdint>

enum class EnemyType : std::uint8_t {
    Alpha = 'A',
    Beta = 'B',
    Gamma = 'G',
    Dragon = 'D',
    Monster = 'M'
};

static_assert(sizeof(EnemyType) == 1);

constexpr char toEnemyTypeCode(EnemyType type) noexcept {
    return static_cast<char>(type);
}
