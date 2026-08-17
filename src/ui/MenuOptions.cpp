#include "ui/Menu.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"

#include <iostream>
#include <utility>

ShipType Menu::showOptions(sf::RenderWindow& window, ShipType currentShip, bool& mouseControl) {
    struct ShipOption {
        ShipType type;
        sf::Sprite sprite;
    };
    std::vector<ShipOption> ships;

    sf::Text title(font, "Options", 60);
    sf::Text shipLabel(font, "Select Ship:", 36);
    sf::Text controlLabel(font, "Controls:", 30);
    MenuButton controlButton(font, mouseControl ? "[Mouse]" : "[Keyboard]", 28);
    MenuButton returnButton(font, "Return");

    centerText(title, 80);
    centerText(shipLabel, 180);
    centerText(controlLabel, 500);
    controlButton.setCenter(GameConfig::WindowWidth, 550.f);
    returnButton.setCenter(GameConfig::WindowWidth, 700.f);

    ShipType selectedShip = currentShip;
    constexpr float shipY = 260.f;
    constexpr float shipSpacing = 250.f;
    const float startX = (GameConfig::WindowWidth - 2.f * shipSpacing) / 2.f;

    const auto addShip = [&](ShipType type, const std::string& textureName, float centerX) {
        try {
            auto& texture = resources.getTexture(textureName);
            sf::Sprite sprite(texture);
            const auto bounds = sprite.getGlobalBounds();
            sprite.setPosition({centerX - bounds.size.x / 2.f, shipY});
            ships.push_back({type, std::move(sprite)});
        } catch (const std::exception& error) {
            std::cerr << "Failed to load ship icon: " << error.what() << '\n';
        }
    };

    addShip(ShipType::Red, "IconShip1_red.png", startX);
    addShip(ShipType::Green, "IconShip2_green.png", startX + shipSpacing);
    addShip(ShipType::Orange, "IconShip3_orange.png", startX + 2.f * shipSpacing);

    sf::RectangleShape selector;
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color::Yellow);
    selector.setOutlineThickness(3.f);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return selectedShip;
            }

            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    for (const auto& ship : ships) {
                        if (ship.sprite.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                            selectedShip = ship.type;
                            break;
                        }
                    }

                    if (controlButton.contains(mousePosition)) {
                        mouseControl = !mouseControl;
                        controlButton.setString(mouseControl ? "[Mouse]" : "[Keyboard]");
                    }
                    if (returnButton.contains(mousePosition)) return selectedShip;
                }
            }
        }

        for (const auto& ship : ships) {
            if (ship.type == selectedShip) {
                const auto bounds = ship.sprite.getGlobalBounds();
                selector.setSize({bounds.size.x + 10.f, bounds.size.y + 10.f});
                selector.setPosition({bounds.position.x - 5.f, bounds.position.y - 5.f});
                break;
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        controlButton.updateHover(mousePosition);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(shipLabel);
        for (const auto& ship : ships) window.draw(ship.sprite);
        window.draw(selector);
        window.draw(controlLabel);
        controlButton.draw(window);
        returnButton.draw(window);
        window.display();
    }
    return selectedShip;
}
