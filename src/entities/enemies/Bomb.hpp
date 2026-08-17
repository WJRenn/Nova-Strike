#pragma once

#include "entities/Entity.hpp"

class ResourceManager;

class Bomb : public Entity {
public:
    Bomb(ResourceManager& resources, float x, float y);
    ~Bomb() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    void setFallSpeed(float speed) { fallSpeed = speed; }
    void setDirection(float dx, float dy) { 
        velocity.x = dx; 
        velocity.y = dy; 
    }
    
    bool isHit() const { return hit; }
    void markHit() { hit = true; destroy(); }

private:
    float fallSpeed = 300.f;  // pixels per second
    bool hit = false;
    
};
