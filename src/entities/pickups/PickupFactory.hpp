#pragma once

#include <memory>
#include <random>

class Pickup;
class ResourceManager;

enum class PickupType {
    Health,
    ScoreBonus,
    Piercing,
    MeteorHazard
};

class PickupFactory {
public:
    explicit PickupFactory(ResourceManager& resources);

    std::unique_ptr<Pickup> create(PickupType type, float x, float y);
    std::unique_ptr<Pickup> createRandom(float x, float y);

private:
    ResourceManager& resources;
    std::mt19937 randomEngine;
};
