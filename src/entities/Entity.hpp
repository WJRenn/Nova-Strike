#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <memory>
#include <cstdint>
#include <vector>

class Entity {
    
public:
    using Id = std::uint64_t;
    Entity() :id(nextId++) {};
    virtual ~Entity() = default;
    Id getId() const { return id; }

    // Core update/draw methods
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    
    // Position management
    sf::Vector2f getPosition() const { return position; }
    void setPosition(const sf::Vector2f& pos) { 
        position = pos; 
        if (sprite) {
            sprite->setPosition(position);
        }
    }
    void setPosition(float x, float y) { 
        setPosition({x, y}); 
    }
    
    // Velocity management
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
    void setVelocity(float vx, float vy) { velocity = {vx, vy}; }
    
    // Collision
    sf::FloatRect getBounds() const { 
        if (sprite) {
            return sprite->getGlobalBounds();
        }
        return sf::FloatRect({position.x, position.y}, {0.f, 0.f});
    }
    bool intersects(const Entity& other) const {
        return getBounds().findIntersection(other.getBounds()).has_value();
    }
    
    // State
    bool isDestroyed() const { return destroyed; }
    void destroy() { destroyed = true; }

protected:
    void initSprite(const sf::Texture& texture) {
        sprite.emplace(texture);
        sprite->setPosition(position);
    }
    
    std::optional<sf::Sprite> sprite;
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    bool destroyed = false;

private:
    Id id;
    inline static Id nextId = 1;
};

