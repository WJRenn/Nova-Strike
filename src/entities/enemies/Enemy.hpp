#pragma once

#include "entities/Entity.hpp"
#include "entities/enemies/EnemyType.hpp"
#include <memory>

class ResourceManager;
class Bomb;

class Enemy : public Entity {
public:
    Enemy(ResourceManager& resources, int health, EnemyType type);
    virtual ~Enemy();
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    // Virtual methods for subclasses - attack patterns
    virtual void onTransform() {}
    virtual float getBombCooldown() const { return 5.f; }
    virtual float getBombSpeed() const { return 300.f; }
    virtual bool hasBombSpread() const { return false; }
    virtual bool hasAimedBombs() const { return false; }
    
    // Movement pattern - override in subclasses
    virtual float getMoveSpeed() const { return 50.f; }
    
    // Combat
    void takeDamage(int amount);
    bool isDead() const { return health <= 0; }
    
    // Bombing
    virtual std::vector<Bomb*> getActiveBombs();
    
    // Getters
    int getHealth() const { return health; }
    EnemyType getType() const { return type; }
    int getScoreValue() const;
    
    // Movement
    void moveBy(float dx, float dy);
    void setTargetPosition(float x, float y);
    void setStartPosition(float x, float y);
    
    // Set player position for aimed bombs
    void setPlayerPosition(float x, float y) { playerX = x; playerY = y; }
    void setHealth(int value) { health = value; }

protected:
    ResourceManager& resources;
    
    int health;
    EnemyType type;
    
    float playerX = 500.f;
    float playerY = 850.f;
    
    // Movement oscillation
    float startX = 0.f;
    float oscillateTimer = 0.f;
    float oscillateRange = 50.f;  // How far to move left/right
    
    void updateMovement(float dt);

private:
    std::unique_ptr<Bomb> bomb;
    float bombTimer = 0.f;

    void tryDropBomb();
    void updateRegularBomb(float dt);
};
