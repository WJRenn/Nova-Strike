#include "gameplay/LevelManager.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"
#include "entities/enemies/Enemy.hpp"
#include "entities/enemies/InvaderTypes.hpp"
#include "entities/enemies/Dragon.hpp"
#include "entities/enemies/Monster.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <array>
#include <algorithm>
namespace {
    constexpr std::array<LevelDefinition, 6> LEVELS{ {
        {
            LevelState::Level1,
            1,
            FormationType::Grid,
            3,
            5,
            15,
            "LEVEL 1"
        },
        {
            LevelState::Level2,
            2,
            FormationType::Circle,
            0,
            0,
            12,
            "LEVEL 2"
        },
        {
            LevelState::Level3,
            3,
            FormationType::Grid,
            4,
            5,
            20,
            "LEVEL 3"
        },
        {
            LevelState::BossDragon,
            4,
            FormationType::Dragon,
            0,
            0,
            1,
            "DRAGON BOSS"
        },
        {
            LevelState::BossMonster,
            5,
            FormationType::Monster,
            0,
            0,
            1,
            "FINAL BOSS: MONSTER"
        },
        {
            LevelState::Victory,
            0,
            FormationType::None,
            0,
            0,
            0,
            "VICTORY"
        }
    } };
}
LevelManager::LevelManager(ResourceManager& res) : resources(res) {}

const LevelDefinition& getDefinition(LevelState state) {
    auto it = std::find_if(
        LEVELS.begin(),
        LEVELS.end(),
        [state](const LevelDefinition& level) {
            return level.state == state;
        }
    );
    return *it;
}
void LevelManager::spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.clear();
    const auto& level = getDefinition(state);
    switch (level.formation) {
        case FormationType::Grid:
            spawnGridFormation(enemies, level.rows, level.columns);  
            break;
        case FormationType::Circle:
            spawnCircleFormation(enemies, level.count);
            break;
        case FormationType::Dragon:
            spawnDragon(enemies);
            break;
        case FormationType::Monster:
            spawnMonster(enemies);
            break;
        case FormationType::None:
            break;
    }
}

bool LevelManager::isLevelComplete(const std::vector<std::unique_ptr<Enemy>>& enemies) const {
    if (state == LevelState::Victory) return false;
    
    for (const auto& enemy : enemies) {
        if (!enemy->isDead() && !enemy->isDestroyed()) {
            return false;
        }
    }
    return !enemies.empty();
}

bool LevelManager::nextLevel() {
    auto it = std::find_if(LEVELS.begin(), LEVELS.end(), [this](const LevelDefinition& level) {
        return level.state == state;
        });

    if (it == LEVELS.end()) return false;
    if (std::next(it) == LEVELS.end()) return false;

    state = std::next(it)->state;
    return true;

}
bool LevelManager::setLevelNumber(int level) {
    auto it = std::find_if(LEVELS.begin(), LEVELS.end(), [level](const LevelDefinition& definition) {
        return definition.displayNumber == level;
        });

    if (it == LEVELS.end()||it->state == LevelState::Victory) return false;
    state = it->state;
    return true;
}
int LevelManager::getLevelNumber() const {
    return getDefinition(state).displayNumber;
}

std::unique_ptr<Enemy> LevelManager::createRandomInvader(float x, float y) {
    int type = std::rand() % 3;
    switch (type) {
        case 0: return std::make_unique<Alpha>(resources, x, y);
        case 1: return std::make_unique<Beta>(resources, x, y);
        default: return std::make_unique<Gamma>(resources, x, y);
    }
}

void LevelManager::spawnGridFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int rows, int cols) {
    float spacingX = 180.f;  // Wide spacing for oscillation
    float spacingY = 140.f;  // Increased vertical spacing
    float startX = (GameConfig::WindowWidth - (cols - 1) * spacingX) / 2.f;
    float startY = 50.f;
    
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            float x = startX + col * spacingX;
            float y = startY + row * spacingY;
            enemies.push_back(createRandomInvader(x, y));
        }
    }
}

void LevelManager::spawnCircleFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int count) {
    float centerX = GameConfig::WindowWidth / 2.f;
    float centerY = 280.f;
    float radius = 250.f;  // Increased from 200
    
    for (int i = 0; i < count; i++) {
        float angle = (2.f * 3.14159f * i) / count;
        float x = centerX + radius * std::cos(angle);
        float y = centerY + radius * std::sin(angle);
        enemies.push_back(createRandomInvader(x, y));
    }
}

void LevelManager::spawnDragon(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.push_back(std::make_unique<Dragon>(resources, GameConfig::WindowWidth / 2.f, 200.f));
}

void LevelManager::spawnMonster(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.push_back(std::make_unique<Monster>(resources, GameConfig::WindowWidth / 2.f, 200.f));
}
