#pragma once

#include "entities/enemies/Enemy.hpp"
#include <vector>

class Dragon : public Enemy {
public:
    Dragon(ResourceManager& resources,float x = 400.f, float y = 50.f);
    ~Dragon() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    void onTransform() override;
    
    // Boss-specific overrides
    float getBombCooldown() const override { return 1.5f; }
    float getBombSpeed() const override { return 350.f; }
    float getMoveSpeed() const override { return 80.f; }
    
    // Dragon fires multiple bombs
    std::vector<Bomb*> getActiveBombs() override;
    
private:
    static constexpr int FRAME_SIZE = 600;
    static constexpr int SHEET_COLS = 2;
    static constexpr int SHEET_ROWS = 2;
    
    // Multiple bombs for spread shot
    std::vector<std::unique_ptr<Bomb>> bombs;
    
    // Swooping movement
    float swoopTimer = 0.f;
    float startY = 0.f;
    float swoopRangeY = 80.f;  // Vertical swoop range

    float attackTimer = 0.f;
};
