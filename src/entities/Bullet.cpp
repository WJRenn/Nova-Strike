#include "entities/Bullet.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"
#include <iostream>
Bullet::Bullet(ResourceManager& resources, float x, float y, const std::string& textureName,bool isPiercing):piercing(isPiercing) {
    setPosition(x, y);
    setVelocity(0.f, -1.f);  // Move upward
    
    try {// Load bullet texture
        auto& texture = resources.getTexture(textureName);
        initSprite(texture);
        
        if (textureName == "fire.png") sprite->setScale({0.2f, 0.2f});// Scale fire texture smaller
        else sprite->setScale({ 0.75f, 0.75f });
        
        sprite->setPosition(position);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load bullet texture: " << e.what() << std::endl;
    }
}
void Bullet::update(float dt) {
    if (destroyed) return;
    // Move bullet
    position += velocity * speed * dt;
    if (sprite) sprite->setPosition(position);
    // Destroy if off screen
    if (position.y < -50.f || position.y > GameConfig::WindowHeight + 50.f) destroy();
}
void Bullet::draw(sf::RenderWindow& window) const {
    if (sprite && !destroyed)   window.draw(*sprite);
}
