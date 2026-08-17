#include "systems/CollisionSystem.hpp"

#include "entities/Bullet.hpp"
#include "entities/Spaceship.hpp"
#include "entities/enemies/Bomb.hpp"
#include "entities/enemies/Enemy.hpp"
#include "entities/pickups/Pickup.hpp"

#include <iostream>

CollisionResult CollisionSystem::resolve(
    Spaceship& player,
    std::vector<std::unique_ptr<Enemy>>& enemies,
    std::vector<std::unique_ptr<Pickup>>& pickups,
    const EnemyDestroyedCallback& onEnemyDestroyed) {
    auto& bullets = player.getBullets();

    for (auto& bullet : bullets) {
        if (bullet->isDestroyed()) continue;
        bullet->beginCollisionFrame();

        for (auto& enemy : enemies) {
            if (enemy->isDestroyed() || enemy->isDead()) continue;
            if (!bullet->intersects(*enemy)) continue;

            const bool shouldDamage = !bullet->isPiercing()
                || bullet->registerOverlap(enemy->getId());
            if (!shouldDamage) continue;

            enemy->takeDamage(1);
            if (enemy->isDead()) {
                player.addScore(enemy->getScoreValue());
                onEnemyDestroyed(enemy->getPosition().x, enemy->getPosition().y);
                std::cout << "Score: " << player.getScore() << std::endl;
            }

            if (!bullet->isPiercing()) {
                bullet->markHit();
                break;
            }
        }
        bullet->endCollisionFrame();
    }

    for (auto& enemy : enemies) {
        for (Bomb* bomb : enemy->getActiveBombs()) {
            if (!bomb || bomb->isDestroyed() || !bomb->intersects(player)) continue;
            bomb->markHit();
            player.damage(1);
            if (player.getHealth() <= 0) return {true};
        }
    }

    for (auto& pickup : pickups) {
        if (!pickup->isDestroyed() && pickup->intersects(player)) {
            pickup->applyEffect(player);
            pickup->destroy();
        }
    }

    return {};
}
