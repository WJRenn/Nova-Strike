#include "entities/enemies/InvaderTypes.hpp"
#include "core/ResourceManager.hpp"

// Alpha - Health 3, Row 0
Alpha::Alpha(ResourceManager& resources, float x, float y) 
    : Invader(resources, 3, EnemyType::Alpha) {
    setStartPosition(x, y);  // Sets both position and oscillation center
    setFrame(0, 0);
    onTransform();
}

// Beta - Health 5, Row 1
Beta::Beta(ResourceManager& resources, float x, float y) 
    : Invader(resources, 5, EnemyType::Beta) {
    setStartPosition(x, y);
    setFrame(0, 1);
    onTransform();
}

// Gamma - Health 7, Row 2
Gamma::Gamma(ResourceManager& resources, float x, float y) 
    : Invader(resources, 7, EnemyType::Gamma) {
    setStartPosition(x, y);
    setFrame(0, 2);
    onTransform();
}
