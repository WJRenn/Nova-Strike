#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include "core/ResourceManager.hpp"
#include "gameplay/LevelManager.hpp"
#include "ui/Menu.hpp"
#include "input/PlayerInput.hpp"
#include "entities/pickups/Pickup.hpp"
#include "entities/pickups/PickupFactory.hpp"
#include "systems/SaveSystem.hpp"

// Forward declarations
class Entity;
class Spaceship;
class Enemy;

enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver,
    Victory
};

class Game {
public:
    Game();
    ~Game();

    void run();
    
    // Access to resources for entities
    ResourceManager& getResources() { return resources; }

private:
    void processEvents();
    void update(float dt);
    void render();

    void handleKeyPress(sf::Keyboard::Key key);
    PlayerInput collectPlayerInput();
    
    // Game logic
    void spawnPickup();
    void checkCollisions();
    void cleanupDestroyedEntities();
    void checkLevelComplete();
    void restartGame();
    void showMainMenu();
    void handleGameOver();
    void handleVictory();

    void renderHUD();
    void spawnExplosion(float x, float y);
    void updateExplosions(float dt);
    void saveGame();
    bool loadGame(); 

private:
    static constexpr float TIME_PER_FRAME = 1.f / 60.f;
    
    // Resources must be declared BEFORE sprites that use them
    ResourceManager resources;
    SaveSystem saveSystem;
    PickupFactory pickupFactory;
    LevelManager levelManager;
    Menu menu;
    
    sf::RenderWindow window;//创建可绘制内容的窗口
    std::optional<sf::Sprite> background;
    
    GameState state;
    bool isRunning;
    
    // Player
    std::unique_ptr<Spaceship> player;
    
    // Enemies
    std::vector<std::unique_ptr<Enemy>> enemies;
    
    // Pickups and hazards
    std::vector<std::unique_ptr<Pickup>> pickups;
    float pickupSpawnTimer = 0.f;
    
    // Settings
    ShipType selectedShip = ShipType::Red; 
    bool useMouseControl = false;  // Mouse vs keyboard
    sf::Font hudFont;
    
    // Explosions (4-frame animation, 552x138 sheet)
    struct Explosion {
        std::optional<sf::Sprite> sprite;
        float timer = 0.f;
        int frame = 0;
        static constexpr int FRAME_COUNT = 4;
        static constexpr int FRAME_SIZE = 138;  
        static constexpr float FRAME_DURATION = 0.1f;
    };
    std::vector<Explosion> explosions;
    
    // Other entities
    std::vector<std::unique_ptr<Entity>> entities;
};
