#include "ui/Menu.hpp"
#include "config/GameConfig.hpp"

MenuResult Menu::showLeaderboard(sf::RenderWindow& window) {
    leaderboard.reload();

    sf::Text title(font, "Leaderboard", 60);
    MenuButton returnButton(font, "Return");
    centerText(title, 80);
    returnButton.setCenter(GameConfig::WindowWidth, 850.f);

    std::vector<sf::Text> entries;
    const auto& leaderboardEntries = leaderboard.getEntries();
    for (std::size_t i = 0; i < leaderboardEntries.size(); ++i) {
        const auto& entryData = leaderboardEntries[i];
        const std::string label = std::to_string(i + 1) + ". "
            + entryData.name + " - " + std::to_string(entryData.score);
        sf::Text entry(font, label, 28);
        centerText(entry, 180.f + static_cast<float>(i) * 60.f);
        entries.push_back(entry);
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
                    if (returnButton.contains(mousePosition)) return MenuResult::Return;
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        for (const auto& entry : entries) window.draw(entry);
        returnButton.draw(window);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showHelp(sf::RenderWindow& window) {
    sf::Text title(font, "Help", 60);
    sf::Text controls(font, "Controls:", 36);
    sf::Text wasd(font, "WASD / Arrows - Move", 28);
    sf::Text space(font, "SPACE - Shoot", 28);
    sf::Text escape(font, "ESC - Pause", 28);
    MenuButton returnButton(font, "Return");

    centerText(title, 100);
    centerText(controls, 220);
    centerText(wasd, 300);
    centerText(space, 360);
    centerText(escape, 420);
    returnButton.setCenter(GameConfig::WindowWidth, 600.f);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    if (returnButton.contains(mousePosition)) return MenuResult::Return;
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(controls);
        window.draw(wasd);
        window.draw(space);
        window.draw(escape);
        returnButton.draw(window);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showAbout(sf::RenderWindow& window) {
    sf::Text title(font, "About", 60);
    sf::Text line1(font, "Nova Strike", 36);
    sf::Text line2(font, "A CPP game using SFML 3.x", 24);
    sf::Text line3(font, "Refined by RWJ", 24);
    sf::Text line4(font, "Rebuilt with modern practices", 24);
    MenuButton returnButton(font, "Return");

    centerText(title, 100);
    centerText(line1, 250);
    centerText(line2, 320);
    centerText(line3, 400);
    centerText(line4, 450);
    returnButton.setCenter(GameConfig::WindowWidth, 650.f);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            if (auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    const auto mousePosition = sf::Mouse::getPosition(window);
                    if (returnButton.contains(mousePosition)) return MenuResult::Return;
                }
            }
        }

        const auto mousePosition = sf::Mouse::getPosition(window);
        returnButton.updateHover(mousePosition);

        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(line1);
        window.draw(line2);
        window.draw(line3);
        window.draw(line4);
        returnButton.draw(window);
        window.display();
    }
    return MenuResult::Quit;
}
