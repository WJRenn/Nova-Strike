#pragma once
#include "entities/Entity.hpp"
#include "entities/Spaceship.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"

class Pickup : public Entity {
public:
    Pickup(ResourceManager& resources, const std::string& textureName, float x, float y)
        : resources(resources)
    {
        const sf::Texture& tex = resources.getTexture(textureName);
        initSprite(tex);
        
        position = {x, y};
        if (sprite)    sprite->setPosition(position);
    }
    
    virtual ~Pickup() = default;
    
    void update(float dt) override {
        if (destroyed) return;
        
        // Fall downwards
        position.y += fallSpeed * dt;
        
        if (sprite)     sprite->setPosition(position);
        
        // Destroy if off screen
        if (position.y > GameConfig::WindowHeight) {
            destroy();
        }
    }
    
    void draw(sf::RenderWindow& window) const override {
        if (sprite && !destroyed)    window.draw(*sprite);
    }
    
    // Virtual effect to be implemented by subclasses
    virtual void applyEffect(Spaceship& player) = 0;
    
protected:
    ResourceManager& resources;
    float fallSpeed = 150.f;  // Default fall speed
};
