#pragma once

#include "Pickup.hpp"

class HealthPickup : public Pickup {
public:
    HealthPickup(ResourceManager& resources, float x, float y)
        : Pickup(resources, "PNG/Power-ups/things_silver.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.addHealth(1);
    }
};
