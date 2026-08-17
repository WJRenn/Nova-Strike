#pragma once

#include "entities/enemies/Invader.hpp"
#include <cstdlib>

// Alpha - Row 0, Health 3, Slow bombs (5s cooldown), straight down, slow movement
class Alpha : public Invader {
public:
    Alpha(ResourceManager& resources, float x = 100.f, float y = 100.f);
    
    void onTransform() override {
        setFrame(rand() % 4, 0);  // Row 0
    }
    
    float getBombCooldown() const override { return 5.f; }
    float getBombSpeed() const override { return 200.f; }  // Slow
    float getMoveSpeed() const override { return 30.f; }   // Slow oscillation
};

// Beta - Row 1, Health 5, Medium bombs (3s cooldown), slight spread, medium movement
class Beta : public Invader {
public:
    Beta(ResourceManager& resources, float x = 100.f, float y = 100.f);
    
    void onTransform() override {
        setFrame(rand() % 4, 1);  // Row 1
    }
    
    float getBombCooldown() const override { return 3.f; }
    float getBombSpeed() const override { return 350.f; }  // Medium
    bool hasBombSpread() const override { return true; }   // Can spread
    float getMoveSpeed() const override { return 60.f; }   // Medium oscillation
};

// Gamma - Row 2, Health 7, Fast bombs (2s cooldown), aimed at player, fast movement
class Gamma : public Invader {
public:
    Gamma(ResourceManager& resources, float x = 100.f, float y = 100.f);
    
    void onTransform() override {
        setFrame(rand() % 4, 2);  // Row 2
    }
    
    float getBombCooldown() const override { return 2.f; }
    float getBombSpeed() const override { return 450.f; }  // Fast
    bool hasAimedBombs() const override { return true; }   // Aims at player
    float getMoveSpeed() const override { return 100.f; }  // Fast oscillation
};
