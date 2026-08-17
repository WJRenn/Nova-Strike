#include "ui/Menu.hpp"
#include "config/GameConfig.hpp"
#include "core/ResourceManager.hpp"

#include <filesystem>
#include <iostream>

MenuButton::MenuButton(const sf::Font& font, const std::string& label, unsigned int size)
    : text(font, label, size)
    , screenWidth(static_cast<float>(GameConfig::WindowWidth)) {
}

void MenuButton::setCenter(float width, float newY) {
    screenWidth = width;
    y = newY;
    updatePosition();
}

void MenuButton::setString(const std::string& value) {
    text.setString(value);
    updatePosition();
}

void MenuButton::updatePosition() {
    const auto bounds = text.getLocalBounds();
    text.setPosition({(screenWidth - bounds.size.x) / 2.f, y});
}

bool MenuButton::contains(const sf::Vector2i& mousePosition) const {
    return text.getGlobalBounds().contains(sf::Vector2f(mousePosition));
}

void MenuButton::updateHover(const sf::Vector2i& mousePosition) {
    text.setFillColor(contains(mousePosition) ? sf::Color::Yellow : sf::Color::White);
}

void MenuButton::draw(sf::RenderWindow& window) const {
    window.draw(text);
}

Menu::Menu(ResourceManager& resourceManager)
    : resources(resourceManager)
    , leaderboard(std::filesystem::path(resourceManager.getBasePath()) / "scores.txt") {
    try {
        font = resources.getFont("Lovelo-LineBold.ttf");
    } catch (const std::exception& error) {
        std::cerr << "Failed to load menu font: " << error.what() << std::endl;
    }

    try {
        background.emplace(resources.getTexture("Start_Menu.png"));
    } catch (const std::exception& error) {
        std::cerr << "Warning: Menu background not loaded: " << error.what() << std::endl;
    }
}

void Menu::centerText(sf::Text& text, float y) {
    const auto bounds = text.getLocalBounds();
    text.setPosition({(GameConfig::WindowWidth - bounds.size.x) / 2.f, y});
}

MenuResult Menu::showMainMenu(sf::RenderWindow& window, bool hasSaveFile) {
    sf::Text title(font, "Nova Strike", 70);
    centerText(title, 125);

    std::vector<MenuItem> items;
    items.push_back({MenuButton(font, "Continue"), MenuResult::Continue, hasSaveFile});
    items.push_back({MenuButton(font, "New Game"), MenuResult::Play});
    items.push_back({MenuButton(font, "Leaderboard"), MenuResult::Leaderboard});
    items.push_back({MenuButton(font, "Options"), MenuResult::Options});
    items.push_back({MenuButton(font, "Help"), MenuResult::Help});
    items.push_back({MenuButton(font, "About"), MenuResult::About});
    items.push_back({MenuButton(font, "Quit"), MenuResult::Quit});

    float buttonY = 260.f;
    for (auto& item : items) {
        if (!item.visible) continue;
        item.button.setCenter(GameConfig::WindowWidth, buttonY);
        buttonY += 60.f;
    }

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    for (auto& item : items) {
                        if (item.visible && item.button.contains(mousePosition)) return item.result;
                    }
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        for (auto& item : items) {
            if (item.visible) item.button.updateHover(mousePosition);
        }

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        for (const auto& item : items) {
            if (item.visible) item.button.draw(window);
        }
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showPauseMenu(sf::RenderWindow& window) {
    sf::Text title(font, "Game Paused", 60);
    centerText(title, 150);

    std::vector<MenuItem> items;
    items.push_back({MenuButton(font, "Resume"), MenuResult::Resume});
    items.push_back({MenuButton(font, "Restart"), MenuResult::Restart});
    items.push_back({MenuButton(font, "Exit to Menu"), MenuResult::Return});

    float buttonY = 320.f;
    for (auto& item : items) {
        item.button.setCenter(GameConfig::WindowWidth, buttonY);
        buttonY += 80.f;
    }

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Escape) return MenuResult::Resume;
            }
            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    for (const auto& item : items) {
                        if (item.visible && item.button.contains(mousePosition)) return item.result;
                    }
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        for (auto& item : items) {
            if (item.visible) item.button.updateHover(mousePosition);
        }

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        for (const auto& item : items) {
            if (item.visible) item.button.draw(window);
        }
        window.display();
    }
    return MenuResult::Quit;
}
