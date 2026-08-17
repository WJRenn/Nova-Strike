#include "app/Game.hpp"
#include "config/GameConfig.hpp"
#include <algorithm>
#include "entities/Entity.hpp"
#include "entities/Spaceship.hpp"
#include "entities/enemies/Enemy.hpp"
#include "entities/enemies/InvaderTypes.hpp"
#include "entities/enemies/Dragon.hpp"
#include "entities/enemies/Monster.hpp"
#include "systems/CollisionSystem.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cmath>

Game::Game()
    : saveSystem(
        std::filesystem::path(resources.getBasePath()) / "savegame.bin",
        Spaceship::getMaxHealth())
    , pickupFactory(resources)
    , levelManager(resources)
    , menu(resources)
    , window(sf::VideoMode({GameConfig::WindowWidth, GameConfig::WindowHeight}), "Nova Strike")
    , state(GameState::Menu)
    , isRunning(true)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));//随机数
    window.setFramerateLimit(60);//设置帧率

    // Load background  背景图
    try {
        auto& bgTexture = resources.getTexture("background_1.png");
        background.emplace(bgTexture);
        background->setScale({2.f, 1.5f});
    } catch (const std::exception& e) {
        std::cerr << "Warning: " << e.what() << std::endl;
    }
    
    // Load HUD font 字体
    try {
        hudFont = resources.getFont("Lovelo-LineBold.ttf");
    } catch (const std::exception& e) {
        std::cerr << "Warning: HUD font not loaded" << std::endl;
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
}

Game::~Game() {
    // Clear sprites BEFORE resources are destroyed
    player.reset();
    enemies.clear();
    background.reset();
    entities.clear();
    resources.clear();
    std::cout << "Game shutting down..." << std::endl;
}

void Game::run() {
    sf::Clock clock;
    float accumulator = 0.f;
    
    while (window.isOpen() && isRunning) {
        // Handle menu states
        if (state == GameState::Menu) {
            showMainMenu();
            clock.restart();
            continue;
        }
        
        if (state == GameState::Paused) {
            auto result = menu.showPauseMenu(window);
            if (result == MenuResult::Resume) {
                state = GameState::Playing;
            } else if (result == MenuResult::Restart) {
                restartGame();
            } else if (result == MenuResult::Return || result == MenuResult::Quit) {
                saveGame();  // Save progress when exiting to menu
                state = GameState::Menu;
            }
            clock.restart();
            continue;
        }
        
        if (state == GameState::GameOver) {
            handleGameOver();
            clock.restart();
            continue;
        }
        if (state == GameState::Victory)
        {
            handleVictory();
            clock.restart();
            continue;
        }

        float dt = clock.restart().asSeconds();
        accumulator += dt;
        
        processEvents();
        
        // Fixed timestep update
        while (accumulator >= TIME_PER_FRAME) {
            if (player) player->setInput(collectPlayerInput());
            update(TIME_PER_FRAME);
            accumulator -= TIME_PER_FRAME;
        }
        
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            handleKeyPress(keyPressed->code);
        }
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
    if (key != sf::Keyboard::Key::Escape)   return;
    
    if (state == GameState::Playing) {
        state = GameState::Paused;
        std::cout << "Game paused" << std::endl;
    } 
    else if (state == GameState::Paused) {
        state = GameState::Playing;
        std::cout << "Game resumed" << std::endl;
    }
    
}

PlayerInput Game::collectPlayerInput() {
    PlayerInput input;
    if (!player || state != GameState::Playing) return input;
    if (useMouseControl)
    {
        const auto mousePixel = sf::Mouse::getPosition(window);
        const auto playerBounds = player->getBounds();
        const sf::Vector2f playerCenter{playerBounds.position.x +playerBounds.size.x / 2.f,playerBounds.position.y +playerBounds.size.y / 2.f};
        const sf::Vector2f mousePosition{static_cast<float>(mousePixel.x),static_cast<float>(mousePixel.y)};
        const sf::Vector2f difference = mousePosition - playerCenter;
        const float length = std::sqrt(difference.x * difference.x + difference.y * difference.y);
        constexpr float DEAD_ZONE = 10.f;
        if (length > DEAD_ZONE) {
            input.moveDirection = {difference.x / length,difference.y / length };
        }
        input.firing = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        return input;
    }
    const bool left =sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);

    const bool right =sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    const bool up =sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);

    const bool down =sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

    input.moveDirection.x =static_cast<float>(right) -static_cast<float>(left);
    input.moveDirection.y =static_cast<float>(down) -static_cast<float>(up);

    const float length = std::sqrt(
        input.moveDirection.x *
        input.moveDirection.x +
        input.moveDirection.y *
        input.moveDirection.y
    );
    if (length > 0.f) {
        input.moveDirection.x /= length;
        input.moveDirection.y /= length;
    }
    input.firing = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    return input;
}


void Game::update(float dt) {
    if (state != GameState::Playing)  return;
    
    // Update player
    if (player) {
        player->update(dt);
        
        // Check for player death
        if (player->getHealth() <= 0) {
            std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
            state = GameState::GameOver;
            return;
        }
    }
    
    // Update explosions
    updateExplosions(dt);    
    // Update enemies (pass player position for aimed bombs)
    for (auto& enemy : enemies) {
        if (player) {
            enemy->setPlayerPosition(player->getPosition().x, player->getPosition().y);
        }
        enemy->update(dt);
    }
    
    // Update other entities
    for (auto& entity : entities) 
        entity->update(dt);
    
    // Update pickups and hazards
    for (auto& pickup : pickups)
        pickup->update(dt);
    
    // Spawn pickups randomly
    pickupSpawnTimer += dt;
    if (pickupSpawnTimer > 5.0f) { // Every 5 seconds roughly
        if (std::rand() % 100 < 40) { // 40% chance every 5s check? or just spawn every 5s?
            spawnPickup();
        }
        pickupSpawnTimer = 0.f;
    }
    
    // Check collisions
    checkCollisions();
    
    // Check level completion
    checkLevelComplete();
    
    // Cleanup destroyed entities
    cleanupDestroyedEntities();
}

void Game::spawnPickup() {
    const float x = static_cast<float>(std::rand() % (GameConfig::WindowWidth - 50));
    constexpr float y = -50.f;

    pickups.push_back(pickupFactory.createRandom(x, y));
}

void Game::checkCollisions() {
    if (!player) return;

    const auto result = CollisionSystem::resolve(
        *player,
        enemies,
        pickups,
        [this](float x, float y) { spawnExplosion(x, y); });

    if (result.playerDefeated) {
        state = GameState::GameOver;
        std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
    }
}

void Game::cleanupDestroyedEntities() {
    // Remove destroyed enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const auto& e) { return e->isDestroyed(); }),
        enemies.end()
    );
    
    // Remove destroyed pickups and hazards
    pickups.erase(
        std::remove_if(pickups.begin(), pickups.end(),
            [](const auto& pickup) { return pickup->isDestroyed(); }),
        pickups.end()
    );
    
    // Remove other destroyed entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const auto& e) { return e->isDestroyed(); }),
        entities.end()
    );
}

void Game::checkLevelComplete() {
    if (state != GameState::Playing) return;
    if (!levelManager.isLevelComplete(enemies)) return; 
    if (!levelManager.nextLevel()) return;
    if (levelManager.isVictory()) {
        enemies.clear();
        pickups.clear();
        saveSystem.deleteSave();
        state = GameState::Victory;
        std::cout << "=== CONGRATULATIONS! YOU WIN! ===" << std::endl;
        std::cout << "Final Score: " << player->getScore() << std::endl;
        return;
    }
    // Spawn next level enemies
    levelManager.spawnLevel(enemies);
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (background)   window.draw(*background);
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        enemy->draw(window);
    }
    
    // Draw pickups and hazards
    for (const auto& pickup : pickups) {
        pickup->draw(window);
    }
    
    // Draw player
    if (player)   player->draw(window);
    
    // Draw other entities
    for (const auto& entity : entities) {
        entity->draw(window);
    }
    
    // Draw explosions
    for (const auto& exp : explosions) {
        if (exp.sprite) window.draw(*exp.sprite);
    }
    
    // Draw HUD
    renderHUD();
    
    window.display();
}

void Game::showMainMenu() {
    // Check if save file exists
    const bool hasSave = saveSystem.exists();
    auto result = menu.showMainMenu(window, hasSave);
    
    switch (result) {
        case MenuResult::Continue:
            if (loadGame())  state = GameState::Playing;
            else {
                const bool shouldDelete = menu.showLoadError(window);
                if (shouldDelete) saveSystem.deleteSave();
            }
            break;
        case MenuResult::Play:
            // Delete old save when starting new game
            saveSystem.deleteSave();
            restartGame();
            break;
        case MenuResult::Leaderboard: {
            menu.showLeaderboard(window);
            break;
        }
        case MenuResult::Help:
            menu.showHelp(window);
            break;
        case MenuResult::About:
            menu.showAbout(window);
            break;
        case MenuResult::Options:
            selectedShip = menu.showOptions(window, selectedShip, useMouseControl);
            break;
        case MenuResult::Quit:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::restartGame() {
    // Reset player with selected ship type
    player = std::make_unique<Spaceship>(resources, selectedShip);
    
    // Reset level manager to level 1 - clear enemies directly
    enemies.clear();
    levelManager.reset();  
    levelManager.spawnLevel(enemies);
    
    // Clear pickups and hazards
    pickups.clear();
    pickupSpawnTimer = 0.f;
    
    state = GameState::Playing;
    std::cout << "=== NEW GAME ===" << std::endl;
}

void Game::handleGameOver() {
    std::string playerName;
    int finalScore = player ? player->getScore() : 0;
    
    auto result = menu.showGameOver(window, finalScore, playerName);
    
    if (result == MenuResult::Return || result == MenuResult::Quit) {
        state = GameState::Menu;
    }
}
void Game::handleVictory(){
    const int finalscore = player ? player->getScore() : 0;
    auto result = menu.showVictory(window, finalscore);
    if (result == MenuResult::Restart){
        restartGame();
    }
    if (result == MenuResult::Return) {
        state = GameState::Menu;
    }
    if (result == MenuResult::Quit)  isRunning = false;
}

void Game::renderHUD() {
    if (!player) return;
    
    // Health bar background
    sf::RectangleShape healthBg({200.f, 20.f});
    healthBg.setPosition({20.f, 20.f});
    healthBg.setFillColor(sf::Color(60, 60, 60));
    
    // Health bar (green to red based on health)
    float healthPercent = static_cast<float>(player->getHealth()) / Spaceship::getMaxHealth();
    sf::RectangleShape healthBar({200.f * healthPercent, 20.f});
    healthBar.setPosition({20.f, 20.f});
    healthBar.setFillColor(healthPercent > 0.5f ? sf::Color::Green : sf::Color::Red);
    
    // Score text
    sf::Text scoreText(hudFont, "Score: " + std::to_string(player->getScore()), 24);
    scoreText.setPosition({GameConfig::WindowWidth - 200.f, 20.f});
    
    // Level text
    sf::Text levelText(hudFont, "Level " + std::to_string(levelManager.getLevelNumber()), 24);
    levelText.setPosition({GameConfig::WindowWidth / 2.f - 50.f, 20.f});
    
    window.draw(healthBg);
    window.draw(healthBar);
    window.draw(scoreText);
    window.draw(levelText);
}

void Game::spawnExplosion(float x, float y) {
    try {
        auto& tex = resources.getTexture("explosion.png");
        Explosion exp;
        exp.sprite.emplace(tex);
        exp.sprite->setPosition({x, y});
        exp.sprite->setScale({0.5f, 0.5f}); 
        exp.timer = Explosion::FRAME_COUNT * Explosion::FRAME_DURATION;  // Total duration
        exp.frame = 0;
        // Set first frame
        exp.sprite->setTextureRect(sf::IntRect({0, 0}, {Explosion::FRAME_SIZE, Explosion::FRAME_SIZE}));
        explosions.push_back(std::move(exp));
    } catch (...) {}
}

void Game::updateExplosions(float dt) {
    for (auto& exp : explosions) {
        exp.timer -= dt;
        
        if (exp.sprite) {
            // Calculate current frame based on elapsed time
            float elapsed = (Explosion::FRAME_COUNT * Explosion::FRAME_DURATION) - exp.timer;
            int frame = static_cast<int>(elapsed / Explosion::FRAME_DURATION);
            frame = std::min(frame, Explosion::FRAME_COUNT - 1);
            
            if (frame != exp.frame) {
                exp.frame = frame;
                exp.sprite->setTextureRect(sf::IntRect(
                    {frame * Explosion::FRAME_SIZE, 0},
                    {Explosion::FRAME_SIZE, Explosion::FRAME_SIZE}
                ));
            }
        }
    }
    // Remove finished explosions
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
            [](const Explosion& e) { return e.timer <= 0; }),
        explosions.end());
}

void Game::saveGame() {
    if (!player) return;

    SavedGame data;
    data.level = static_cast<std::uint32_t>(levelManager.getLevelNumber());
    data.score = static_cast<std::int32_t>(player->getScore());
    data.health = static_cast<std::int32_t>(player->getHealth());
    data.shipType = static_cast<std::uint8_t>(selectedShip);
    data.mouseControl = useMouseControl ? 1 : 0;
    data.playerX = player->getPosition().x;
    data.playerY = player->getPosition().y;
    data.enemies.reserve(enemies.size());

    for (const auto& enemy : enemies) {
        data.enemies.push_back({
            enemy->getType(),
            enemy->getPosition().x,
            enemy->getPosition().y,
            static_cast<std::int32_t>(enemy->getHealth())
        });
    }

    if (saveSystem.save(data)) std::cout << "Game saved successfully\n";
}

bool Game::loadGame() {
    const auto savedGame = saveSystem.load();
    if (!savedGame) return false;
    const auto& data = *savedGame;

    const ShipType loadedShip = static_cast<ShipType>(data.shipType);
    auto loadedPlayer = std::make_unique<Spaceship>(resources, loadedShip);
    loadedPlayer->setScore(data.score);
    loadedPlayer->setHealth(data.health);
    loadedPlayer->setPosition(data.playerX, data.playerY);

    std::vector<std::unique_ptr<Enemy>> loadedEnemies;
    loadedEnemies.reserve(data.enemies.size());
    for (const auto& savedEnemy : data.enemies) {
        std::unique_ptr<Enemy> enemy;
        switch (savedEnemy.type) {
        case EnemyType::Alpha:
            enemy = std::make_unique<Alpha>(resources, savedEnemy.x, savedEnemy.y);
            break;
        case EnemyType::Beta:
            enemy = std::make_unique<Beta>(resources, savedEnemy.x, savedEnemy.y);
            break;
        case EnemyType::Gamma:
            enemy = std::make_unique<Gamma>(resources, savedEnemy.x, savedEnemy.y);
            break;
        case EnemyType::Dragon:
            enemy = std::make_unique<Dragon>(resources, savedEnemy.x, savedEnemy.y);
            break;
        case EnemyType::Monster:
            enemy = std::make_unique<Monster>(resources, savedEnemy.x, savedEnemy.y);
            break;
        }
        if (!enemy) return false;
        enemy->setHealth(savedEnemy.health);
        loadedEnemies.push_back(std::move(enemy));
    }
    if (!levelManager.setLevelNumber(static_cast<int>(data.level))) {
        return false;
    }
    useMouseControl = (data.mouseControl != 0);
    selectedShip = loadedShip;
    player = std::move(loadedPlayer);
    enemies = std::move(loadedEnemies);

    pickups.clear();
    pickupSpawnTimer = 0.f;
    
    std::cout << "Game loaded!\n" << std::endl;
    return true;
}
