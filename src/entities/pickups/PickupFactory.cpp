#include "entities/pickups/PickupFactory.hpp"

#include "Pickup.hpp"
#include "HealthPickup.hpp"
#include "MeteorHazard.hpp"
#include "PiercingPickup.hpp"
#include "ScorePickup.hpp"

#include <array>
#include <stdexcept>

namespace {
    struct SpawnEntry {
        PickupType type;
        int weight;
    };

    // Change these weights to tune the relative spawn chance of each item.
    constexpr std::array<SpawnEntry, 4> PICKUP_SPAWN_TABLE{{
        {PickupType::Health,       25},
        {PickupType::ScoreBonus,   25},
        {PickupType::Piercing,     25},
        {PickupType::MeteorHazard, 25}
    }};
}

PickupFactory::PickupFactory(ResourceManager& resourceManager)
    : resources(resourceManager),
      randomEngine(std::random_device{}())
{
}

std::unique_ptr<Pickup> PickupFactory::create(
    PickupType type,
    float x,
    float y
) {
    switch (type) {
        case PickupType::Health:
            return std::make_unique<HealthPickup>(resources, x, y);
        case PickupType::ScoreBonus:
            return std::make_unique<ScorePickup>(resources, x, y);
        case PickupType::Piercing:
            return std::make_unique<PiercingPickup>(resources, x, y);
        case PickupType::MeteorHazard:
            return std::make_unique<MeteorHazard>(resources, x, y);
    }

    throw std::invalid_argument("Unknown pickup type");
}

std::unique_ptr<Pickup> PickupFactory::createRandom(float x, float y) {
    int totalWeight = 0;
    for (const auto& entry : PICKUP_SPAWN_TABLE) {
        if (entry.weight > 0) totalWeight += entry.weight;
    }

    if (totalWeight <= 0)     throw std::logic_error("Pickup weight total must be positive");

    std::uniform_int_distribution<int> distribution(1, totalWeight);
    int roll = distribution(randomEngine);

    for (const auto& entry : PICKUP_SPAWN_TABLE) {
        if (entry.weight <= 0) continue;

        roll -= entry.weight;
        if (roll <= 0) return create(entry.type, x, y);
    }

    throw std::logic_error("Failed to select a pickup");
}
