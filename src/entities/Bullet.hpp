#pragma once

#include "entities/Entity.hpp"
#include <unordered_set>
class ResourceManager;

class Bullet : public Entity {
public:
    Bullet(ResourceManager& resources, float x, float y, const std::string& textureName = "PNG/Lasers/laserBlue01.png", bool piercing = false);
    ~Bullet() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    bool isHit() const { return hit; }
    void markHit() { hit = true; destroy(); }

    bool isPiercing() const {
        return piercing;
    }

    void beginCollisionFrame() {
        currentOverlaps.clear();
    }

    bool registerOverlap(Entity::Id targetId) {
        currentOverlaps.insert(targetId);

        return previousOverlaps.find(targetId) ==
            previousOverlaps.end();
    }

    void endCollisionFrame() {
        previousOverlaps.swap(currentOverlaps);
        currentOverlaps.clear();
    }

private:
    float speed = 600.f;  // pixels per second
    bool hit = false;
    
    bool piercing = false;

    std::unordered_set<Entity::Id> previousOverlaps;
    std::unordered_set<Entity::Id> currentOverlaps;
};
