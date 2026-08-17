#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include "ui/Leaderboard.hpp"
#include "entities/Spaceship.hpp"
class ResourceManager;

enum class MenuResult {
    None,
    Play,
    Continue,
    Resume,
    Restart,
    Leaderboard,
    Options,
    Help,
    About,
    Quit,
    Return
};
class MenuButton {
public:
    MenuButton(
        const sf::Font& font,
        const std::string& label,
        unsigned int size = 36
    );

    void setCenter(float screenWidth, float y);
    void setString(const std::string& value);

    bool contains(const sf::Vector2i& mousePosition) const;
    void updateHover(const sf::Vector2i& mousePosition);
    void draw(sf::RenderWindow& window) const;

private:
    void updatePosition();

    sf::Text text;
    float screenWidth = 0.f;
    float y = 0.f;
};
class Menu {
public:
    Menu(ResourceManager& resources);
    
    // Show different menu screens - returns when user makes selection
    MenuResult showMainMenu(sf::RenderWindow& window, bool hasSaveFile);
    MenuResult showPauseMenu(sf::RenderWindow& window);
    MenuResult showGameOver(sf::RenderWindow& window, int score, std::string& outPlayerName);
    MenuResult showVictory(sf::RenderWindow& window, int finalScore);
    MenuResult showLeaderboard(sf::RenderWindow& window);
    MenuResult showHelp(sf::RenderWindow& window);
    MenuResult showAbout(sf::RenderWindow& window);
    ShipType showOptions(sf::RenderWindow& window, ShipType currentShip, bool& mouseControl); 

    bool showLoadError(sf::RenderWindow& window);
private:
    void centerText(sf::Text& text, float y);

    ResourceManager& resources;
    Leaderboard leaderboard;
    sf::Font font;
    std::optional<sf::Sprite> background;
    
    struct MenuItem {
        MenuButton button;
        MenuResult result;
        bool visible = true;
    };
};
