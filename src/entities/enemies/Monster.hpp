#pragma once

#include "entities/enemies/Enemy.hpp"
#include <vector>

class Monster : public Enemy {
public:
    Monster(ResourceManager& resources, float x = 400.f, float y = 100.f);
    ~Monster() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    // Boss-specific overrides
    float getBombCooldown() const override { return 1.5f; }  // Aggressive
    float getBombSpeed() const override { return 450.f; }    // Fast tracking beams
    float getMoveSpeed() const override { return 0.f; }
    
    // Monster fires multiple tracking bombs
    std::vector<Bomb*> getActiveBombs() override;
    
private:
    // Multiple tracking bombs
    std::vector<std::unique_ptr<Bomb>> bombs;
    
    // Vertical bobbing movement
    float bobTimer = 0.f;
    float startY = 0.f;
    float bobRange = 50.f;
    float attackTimer = 0.f;
};
