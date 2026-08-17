#pragma once

#include "Pickup.hpp"

class PiercingPickup : public Pickup {
public:
    PiercingPickup(ResourceManager& resources, float x, float y)
        : Pickup(resources, "PNG/Power-ups/powerupGreen_bolt.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.activatePiercingEffect();
    }
};
