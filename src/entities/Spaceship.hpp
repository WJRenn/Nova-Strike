#pragma once

#include "entities/Entity.hpp"
#include "input/PlayerInput.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include<cstdint>

class Bullet;
class ResourceManager;
enum class ShipType :std::uint8_t {
    Red = 1,
    Green = 2,
    Orange = 3
};

class Spaceship : public Entity {
public:
    Spaceship(ResourceManager& resources, ShipType shipType = ShipType::Red);
    ~Spaceship() override;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    void setInput(const PlayerInput& input) { currentInput = input; }
    
    // Getters
    int getHealth() const { return health; }
    int getScore() const { return score; }
    std::vector<std::unique_ptr<Bullet>>& getBullets() { return bullets; }
    
    // Setters
    void addScore(int points);
    void damage(int amount);
    void addHealth(int amount);
    void activatePiercingEffect();
    bool hasPiercingBullets() const { return piercingBullets; }
    void setScore(int s) { score = s; }
    void setHealth(int h) { health = std::min(h, MAX_HEALTH); }
    static constexpr int getMaxHealth() {
        return MAX_HEALTH;
    }
private:
    void wrapAroundScreen();
    void updateBullets(float dt);
    void fire();
    
private:
    ResourceManager& resources;
    std::vector<std::unique_ptr<Bullet>> bullets;
    sf::Clock fireCooldown;
    
    float maxSpeed = 400.f;      // max pixels per second
    float acceleration = 800.f;  // acceleration rate
    float friction = 600.f;      // deceleration when no input
    int health = 3;
    int score = 0;
    bool piercingBullets = false;
    float piercingEffectTimer = 0.f;
    
    static constexpr float FIRE_COOLDOWN_MS = 200.f;
    static constexpr int MAX_HEALTH = 3;
    PlayerInput currentInput;
};
