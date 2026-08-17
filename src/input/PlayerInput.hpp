#pragma once
#include <SFML/Graphics.hpp>

struct PlayerInput {
	sf::Vector2f moveDirection{};
	bool firing = false;
};