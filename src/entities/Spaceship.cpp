#include "entities/Spaceship.hpp"
#include "entities/Bullet.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

Spaceship::Spaceship(ResourceManager& res,ShipType shipType) : resources(res) {
    // Ship textures 
    std::string textureName;
    switch (shipType) {
        case ShipType::Red: textureName = "playerShip1_red.png"; break;
        case ShipType::Green: textureName = "playerShip2_green.png"; break;
        case ShipType::Orange: textureName = "playerShip3_orange.png"; break;
        default: textureName = "playerShip1_red.png"; break;
    }
    
    // Load texture and create sprite
    try {
        auto& texture = resources.getTexture(textureName);
        initSprite(texture);
        auto texSize = texture.getSize();
        if (texSize.x > 150) {  // Sprite sheet - use first frame
            sprite->setTextureRect(sf::IntRect({0, 0}, {131, 131}));
        }
        sprite->setScale({0.75f, 0.75f});
    } catch (const std::exception& e) {
        std::cerr << "Failed to load spaceship texture: " << e.what() << std::endl;
    }
    
    // Start position (center bottom)
    setPosition(GameConfig::WindowWidth / 2.f - 50.f, GameConfig::WindowHeight - 150.f);
}

void Spaceship::update(float dt) {
    if (health <= 0) return;
    
    // Handle the temporary piercing effect
    if (piercingBullets && piercingEffectTimer > 0.f) {
        piercingEffectTimer -= dt;
        if (piercingEffectTimer <= 0.f) {
            piercingBullets = false;
            std::cout << "Piercing effect expired!" << std::endl;
        }
    }
    
    float dx = currentInput.moveDirection.x;
    float dy = currentInput.moveDirection.y;
    
    // Store input direction for animation
    float inputDx = dx, inputDy = dy;
    
    // Apply acceleration towards input direction
    if (dx != 0.f || dy != 0.f) {
        // Accelerate towards input direction
        velocity.x += dx * acceleration * dt;
        velocity.y += dy * acceleration * dt;
    } else {
        // Apply friction when no input (decelerate)
        float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (currentSpeed > 0.f) {
            float frictionAmount = friction * dt;
            if (frictionAmount >= currentSpeed) {
                velocity = {0.f, 0.f};
            } else {
                float scale = (currentSpeed - frictionAmount) / currentSpeed;
                velocity.x *= scale;
                velocity.y *= scale;
            }
        }
    }
    
    // Clamp to max speed
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > maxSpeed) {
        float scale = maxSpeed / currentSpeed;
        velocity.x *= scale;
        velocity.y *= scale;
    }
    
    // Animate sprite based on input direction (131x131 pixel frames)
    if (sprite) {
        const int FRAME = 131;
        if (inputDx < 0 && inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({0, 2 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0 && inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({2 * FRAME, 4 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx < 0 && inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({FRAME, FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0 && inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({2 * FRAME, 0}, {FRAME, FRAME}));
        } else if (inputDx < 0) {
            sprite->setTextureRect(sf::IntRect({3 * FRAME, 2 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0) {
            sprite->setTextureRect(sf::IntRect({3 * FRAME, FRAME}, {FRAME, FRAME}));
        } else if (inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({0, 0}, {FRAME, FRAME}));
        } else if (inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({4 * FRAME, 0}, {FRAME, FRAME}));
        }
        // Keep current frame if no input (momentum continues)
    }
    
    // Fire with space
    if (currentInput.firing)    fire();
    
    // Move based on velocity (already scaled, no speed multiplier needed)
    position += velocity * dt;
    
    // Update sprite position
    if (sprite)  sprite->setPosition(position);
    
    // Wrap around screen
    wrapAroundScreen();
    
    // Update bullets
    updateBullets(dt);
}

void Spaceship::fire() {
    if (fireCooldown.getElapsedTime().asMilliseconds() >= FIRE_COOLDOWN_MS && health > 0) {
        if (piercingBullets) {
            // Fire a single centered piercing projectile
            auto bullet = std::make_unique<Bullet>(resources, position.x - 15.f, position.y - 180.f, "fire.png",true);
            bullets.push_back(std::move(bullet));
        } else {
            // Create two bullets (left and right of ship)
            auto bullet1 = std::make_unique<Bullet>(resources, position.x + 30.f, position.y - 10.f);
            auto bullet2 = std::make_unique<Bullet>(resources, position.x + 62.f, position.y - 10.f);
            bullets.push_back(std::move(bullet1));
            bullets.push_back(std::move(bullet2));
        }
        fireCooldown.restart();
    }
}

void Spaceship::updateBullets(float dt) {
    for (auto& bullet : bullets) 
        bullet->update(dt);
    
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const auto& b) { return b->isDestroyed(); }),
        bullets.end()
    );
}

void Spaceship::wrapAroundScreen() {
    float width = sprite ? sprite->getGlobalBounds().size.x : 75.f;
    float height = sprite ? sprite->getGlobalBounds().size.y : 75.f;
    
    if (position.x < -width) {
        position.x = static_cast<float>(GameConfig::WindowWidth);
    } else if (position.x > GameConfig::WindowWidth) {
        position.x = -width;
    }
    
    if (position.y < -height) {
        position.y = static_cast<float>(GameConfig::WindowHeight);
    } else if (position.y > GameConfig::WindowHeight) {
        position.y = -height;
    }
    
    if (sprite)    sprite->setPosition(position);
}

void Spaceship::draw(sf::RenderWindow& window) const {
    if (sprite && health > 0)    window.draw(*sprite);
    
    for (const auto& bullet : bullets) {
        bullet->draw(window);
    }
}

void Spaceship::damage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
    std::cout << "Health: " << health << std::endl;
}

void Spaceship::addHealth(int amount) {
    health = std::min(health + amount, MAX_HEALTH);
    std::cout << "Health Up! HP: " << health << std::endl;
}

void Spaceship::activatePiercingEffect() {
    piercingBullets = true;
    piercingEffectTimer = 5.0f;
    std::cout << "Piercing bullets active for 5 seconds!" << std::endl;
}

void Spaceship::addScore(int points) { 
    score += points; 
    std::cout << "Score Up! +" << points << " Total: " << score << std::endl;
}

Spaceship::~Spaceship() {
    bullets.clear();
}
