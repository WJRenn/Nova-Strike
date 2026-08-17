#pragma once
#include "Pickup.hpp"
#include <cstdlib>
#include <string>

class MeteorHazard : public Pickup {
public:
    MeteorHazard(ResourceManager& resources, float x, float y)
        : Pickup(resources, getRandomMeteorTexture(), x, y)
    {
        // Meteors fall faster
        fallSpeed = 250.f;
        
        // Random rotation speed
        rotationSpeed = (std::rand() % 200) - 100.f; // -100 to 100
        
        // Center origin for rotation
        if (sprite) {
            auto bounds = sprite->getLocalBounds();
            sprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        }
    }
    
    void update(float dt) override {
        Pickup::update(dt);
        if (sprite) {
            sprite->rotate(sf::degrees(rotationSpeed * dt));
        }
    }
    
    void applyEffect(Spaceship& player) override {
        player.damage(1);
    }
    
private:
    float rotationSpeed;
    
    static std::string getRandomMeteorTexture() {
        // Simple selection of available meteor textures
        const char* textures[] = {
            "PNG/Meteors/meteorBrown_big1.png",
            "PNG/Meteors/meteorBrown_med1.png",
            "PNG/Meteors/meteorGrey_big1.png",
            "PNG/Meteors/meteorGrey_med1.png"
        };
        int index = std::rand() % 4;
        return textures[index];
    }
};
