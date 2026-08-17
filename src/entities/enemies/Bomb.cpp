#include "entities/enemies/Bomb.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"
#include <iostream>

Bomb::Bomb(ResourceManager& resources, float x, float y) {
    setPosition(x, y);
    setVelocity(0.f, 1.f);  // Fall downward
    
    try {
        auto& texture = resources.getTexture("enemy_laser.png");
        initSprite(texture);
        sprite->setPosition(position);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load bomb texture: " << e.what() << std::endl;
    }
}

void Bomb::update(float dt) {
    if (destroyed) return;
    
    position += velocity * fallSpeed * dt;
    
    if (sprite)    sprite->setPosition(position);
    
    // Destroy if off screen (any direction for angled bombs)
    if (position.y > GameConfig::WindowHeight + 50.f || position.y < -50.f ||
        position.x < -50.f || position.x > GameConfig::WindowWidth + 50.f) {
        destroy();
    }
}

void Bomb::draw(sf::RenderWindow& window) const {
    if (sprite && !destroyed)    window.draw(*sprite);
}
