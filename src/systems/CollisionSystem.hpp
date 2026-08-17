#pragma once

#include <functional>
#include <memory>
#include <vector>

class Enemy;
class Pickup;
class Spaceship;

struct CollisionResult {
    bool playerDefeated = false;
};

class CollisionSystem {
public:
    using EnemyDestroyedCallback = std::function<void(float, float)>;

    [[nodiscard]] static CollisionResult resolve(
        Spaceship& player,
        std::vector<std::unique_ptr<Enemy>>& enemies,
        std::vector<std::unique_ptr<Pickup>>& pickups,
        const EnemyDestroyedCallback& onEnemyDestroyed);
};
