#pragma once

#include "Pickup.hpp"

class ScorePickup : public Pickup {
public:
    ScorePickup(ResourceManager& resources, float x, float y)
        : Pickup(resources, "PNG/Power-ups/star_gold.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.addScore(50);
    }
};
