#include "ui/Menu.hpp"
#include "config/GameConfig.hpp"

MenuResult Menu::showGameOver(sf::RenderWindow& window, int score, std::string& outPlayerName) {
    sf::Text title(font, "Game Over", 60);
    sf::Text scoreText(font, "Score: " + std::to_string(score), 40);
    sf::Text prompt(font, "Enter Your Name:", 30);
    sf::Text nameText(font, "", 36);
    MenuButton submitButton(font, "Submit");
    MenuButton returnButton(font, "Return");

    centerText(title, 150);
    centerText(scoreText, 250);
    centerText(prompt, 350);
    centerText(nameText, 440);
    submitButton.setCenter(GameConfig::WindowWidth, 550.f);
    returnButton.setCenter(GameConfig::WindowWidth, 650.f);

    std::string playerName;
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }

            if (auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode < 128 && textEvent->unicode != 8 && textEvent->unicode != 13) {
                    if (playerName.length() < 15) {
                        playerName += static_cast<char>(textEvent->unicode);
                        nameText.setString(playerName);
                        centerText(nameText, 440);
                    }
                } else if (textEvent->unicode == 8 && !playerName.empty()) {
                    playerName.pop_back();
                    nameText.setString(playerName);
                    centerText(nameText, 440);
                }
            }

            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Enter && !playerName.empty()) {
                    outPlayerName = playerName;
                    leaderboard.addScore(playerName, score);
                    return MenuResult::Return;
                }
            }

            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    if (submitButton.contains(mousePosition) && !playerName.empty()) {
                        outPlayerName = playerName;
                        leaderboard.addScore(playerName, score);
                        return MenuResult::Return;
                    }
                    if (returnButton.contains(mousePosition)) return MenuResult::Return;
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        submitButton.updateHover(mousePosition);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(scoreText);
        window.draw(prompt);
        window.draw(nameText);
        submitButton.draw(window);
        returnButton.draw(window);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showVictory(sf::RenderWindow& window, int finalScore) {
    sf::Text title(font, "VICTORY!", 70);
    sf::Text score(font, "Final Score:" + std::to_string(finalScore), 36);
    std::vector<MenuItem> items;
    items.push_back({MenuButton(font, "Restart"), MenuResult::Restart});
    items.push_back({MenuButton(font, "Main Menu"), MenuResult::Return});

    centerText(title, 80);
    centerText(score, 200.f);
    float buttonY = 450.f;
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
            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    for (const auto& item : items) {
                        if (item.button.contains(mousePosition)) return item.result;
                    }
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        for (auto& item : items) item.button.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(score);
        for (const auto& item : items) item.button.draw(window);
        window.display();
    }
    return MenuResult::Quit;
}

bool Menu::showLoadError(sf::RenderWindow& window) {
    sf::Text title(font, "Load Failed", 60);
    sf::Text message(font, "The save file is damaged or incompatible.", 26);
    MenuButton deleteButton(font, "Delete Save");
    MenuButton returnButton(font, "Return");

    centerText(title, 180.f);
    centerText(message, 300.f);
    deleteButton.setCenter(GameConfig::WindowWidth, 450.f);
    returnButton.setCenter(GameConfig::WindowWidth, 540.f);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return false;
            }

            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button != sf::Mouse::Button::Left) continue;
                const auto mousePosition = sf::Mouse::getPosition(window);
                if (deleteButton.contains(mousePosition)) return true;
                if (returnButton.contains(mousePosition)) return false;
            }

            if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Escape) return false;
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        deleteButton.updateHover(mousePosition);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(message);
        deleteButton.draw(window);
        returnButton.draw(window);
        window.display();
    }
    return false;
}
