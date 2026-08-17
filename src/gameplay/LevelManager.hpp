#pragma once

#include <vector>
#include <memory>

class Enemy;
class ResourceManager;

enum class LevelState {
    Level1,
    Level2,
    Level3,
    BossDragon,
    BossMonster,
    Victory
};

enum class FormationType {
    Grid,
    Circle,
    Dragon,
    Monster,
    None
};

struct LevelDefinition {
    LevelState state;
    int displayNumber;
    FormationType formation;
    int rows;
    int columns;
    int count;
    const char* title;
};

class LevelManager {
public:
    LevelManager(ResourceManager& resources);
    
    // Spawn enemies for current level
    void spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies);
    
    // Check if level is complete (all enemies dead)
    bool isLevelComplete(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    
    // Advance to next level
    bool nextLevel();
    bool setLevelNumber(int level);
    // Get current level state
    LevelState getState() const { return state; }
    int getLevelNumber() const;
    bool isVictory() const { return state == LevelState::Victory; }
    void reset() { state = LevelState::Level1; }
    void advanceLevel() { nextLevel(); }  // For save/load
    
private:
    void spawnGridFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int rows, int cols);
    void spawnCircleFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int count);
    void spawnDragon(std::vector<std::unique_ptr<Enemy>>& enemies);
    void spawnMonster(std::vector<std::unique_ptr<Enemy>>& enemies);
    
    std::unique_ptr<Enemy> createRandomInvader(float x, float y);
    
    ResourceManager& resources;
    LevelState state = LevelState::Level1;
    
};
