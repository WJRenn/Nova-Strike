#include "entities/enemies/Dragon.hpp"
#include "entities/enemies/Bomb.hpp"
#include "core/ResourceManager.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Dragon::Dragon(ResourceManager& resources, float x, float y)
    : Enemy(resources, 100, EnemyType::Dragon)
{
    const sf::Texture& tex = resources.getTexture("dragons.png");
    initSprite(tex);
    
    // Set initial random frame from 2x2 sprite sheet
    onTransform();
    
    // Scale down from 600x600 to more reasonable size
    sprite->setScale({0.3f, 0.3f});
    
    position = {x, y};
    sprite->setPosition(position);
    
    // Set start position for both horizontal and vertical oscillation
    setStartPosition(x, y);
    startY = y;
    oscillateRange = 250.f;  // Wide horizontal sweep
}

void Dragon::onTransform() {
    if (!sprite) return;
    
    // Random frame from 2x2 grid (600x600 each)
    int col = std::rand() % SHEET_COLS;
    int row = std::rand() % SHEET_ROWS;
    
    sprite->setTextureRect(sf::IntRect(
        {col * FRAME_SIZE, row * FRAME_SIZE},
        {FRAME_SIZE, FRAME_SIZE}
    ));
}

void Dragon::update(float dt) {
    if (destroyed || health <= 0) return;
    updateMovement(dt);
    // Additional vertical swooping movement
    swoopTimer += dt;
    position.y = startY + std::sin(swoopTimer * 1.5f) * swoopRangeY;
    
    if (sprite)   sprite->setPosition(position);
    
    // Update all active bombs
    for (auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            b->update(dt);
        }
    }
    
    // Clean up destroyed bombs
    bombs.erase(
        std::remove_if(bombs.begin(), bombs.end(),
            [](const auto& b) { return !b || b->isDestroyed(); }),
        bombs.end()
    );
    
    // Aggressive 5-way spread attack
    attackTimer += dt;
    if (attackTimer >= getBombCooldown()) {
        attackTimer -= getBombCooldown();
        
        float baseX = position.x + 90.f;  // Center of dragon
        float baseY = position.y + 150.f;
        float speed = getBombSpeed();
        
        // 5-way spread: far-left, left, center, right, far-right
        float angles[] = {-0.6f, -0.3f, 0.f, 0.3f, 0.6f};
        
        for (float angle : angles) {
            auto bomb = std::make_unique<Bomb>(resources, baseX + angle * 40.f, baseY);
            bomb->setFallSpeed(speed);
            bomb->setDirection(angle, 1.f);
            bombs.push_back(std::move(bomb));
        }
    }
}

void Dragon::draw(sf::RenderWindow& window) const {
    // Draw dragon sprite
    if (sprite && !destroyed && health > 0)   window.draw(*sprite);
    
    // Draw all bombs
    for (const auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            b->draw(window);
        }
    }
}

std::vector<Bomb*> Dragon::getActiveBombs() {
    std::vector<Bomb*> result;
    for (auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            result.push_back(b.get());
        }
    }
    return result;
}
