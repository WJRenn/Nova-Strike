#include "entities/enemies/Invader.hpp"
#include "core/ResourceManager.hpp"
#include <cstdlib>
#include <iostream>

Invader::Invader(ResourceManager& res, int hp, EnemyType t)
    : Enemy(res, hp, t) {
    
    try {
        auto& texture = resources.getTexture("Invaders.png");
        initSprite(texture);
        // Will be set by subclass
    } catch (const std::exception& e) {
        std::cerr << "Failed to load invader texture: " << e.what() << std::endl;
    }
}

void Invader::onTransform() {
    // Random frame animation - override in subclass for specific row
    int col = rand() % 4;
    setFrame(col, 0);
}

void Invader::setFrame(int col, int row) {
    if (sprite) {
        sprite->setTextureRect(sf::IntRect(
            {col * FRAME_SIZE, row * FRAME_SIZE}, 
            {FRAME_SIZE, FRAME_SIZE}
        ));
    }
}
