#include "SFMLUI.h"
#include <iostream>
#include <algorithm>
#include <optional>
#include <filesystem>

SFMLUI::SFMLUI(int width, int height, int cellSize)
    : gridWidth(width), gridHeight(height), baseCellSize(cellSize),
      cellSize(static_cast<float>(cellSize)), gridOffsetX(0), gridOffsetY(0),
      currentState(GameState::HOME_SCREEN)
{
    window.create(
        sf::VideoMode({static_cast<unsigned int>(800), 
                      static_cast<unsigned int>(600)}), 
        "Game of Life",
        sf::Style::Default
    );

    if (window.isOpen()) {
        window.setFramerateLimit(60);
        initializeHomeScreen();
        window.setMinimumSize(sf::Vector2u(400, 300));
    }
}

bool SFMLUI::isMouseOver(const sf::RectangleShape& button) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    return button.getGlobalBounds().contains(worldPos);
}

void SFMLUI::initializeHomeScreen() {
    // Try loading the font from likely resource locations. There's no fatal
    // error here: if the font fails to load we'll still create texts which
    // will use the default font behaviour (empty font results in invisible text
    // in SFML, so it's recommended to ensure the path is correct).
    // Try multiple likely locations for the font file. The executable's
    // working directory can vary depending on how you run the program, so
    // check a few relative locations (project tree, build dir, etc.).
    // Also try to build a path relative to this source file at compile time.
    namespace fs = std::filesystem;

    std::vector<fs::path> candidates;
    candidates.emplace_back("resources/fonts/Pixel-font.ttf");
    candidates.emplace_back("../resources/fonts/Pixel-font.ttf");
    candidates.emplace_back("../../resources/fonts/Pixel-font.ttf");

    // Path based on the source file location (compile-time). Move up from
    // src/UI to project root and append resources/fonts.
    fs::path srcPath = fs::path(__FILE__);
    if (srcPath.has_parent_path()) {
        fs::path projectRoot = srcPath.parent_path().parent_path().parent_path();
        candidates.emplace_back(projectRoot / "resources" / "fonts" / "Pixel-font.ttf");
    }

    bool loaded = false;
    for (const auto& p : candidates) {
        try {
            auto abs = fs::absolute(p);
            if (font.openFromFile(p.string())) {
                loaded = true;
                break;
            } else {
                std::cerr << "Failed to load font (failed to open file): " << p.string() << std::endl;
                std::cerr << "    Absolute path: " << abs.string() << std::endl;
            }
        } catch (const std::exception& ex) {
            std::cerr << "Exception while checking font path: " << ex.what() << std::endl;
        }
    }

    if (!loaded) {
        std::cerr << "Failed to load Pixel-font.ttf from any candidate path. Text may be invisible." << std::endl;
    }
    
    titleText = sf::Text(font, "GAME OF LIFE", 90);
    titleText->setFillColor(sf::Color(200, 200, 200));
    
    playButtonText = sf::Text(font, "PLAY", 36);
    playButtonText->setFillColor(sf::Color(0, 255, 0));
    
    exitButtonText = sf::Text(font, "QUIT", 36);
    exitButtonText->setFillColor(sf::Color(255, 0, 0));
    
    playButton.setSize({200, 60});
    playButton.setFillColor(sf::Color::Transparent);
    
    exitButton.setSize({200, 60});
    exitButton.setFillColor(sf::Color::Transparent);

    // Ensure view and text/button positions are correct before first draw
    updateView();
}

void SFMLUI::updateView() {
    if (!window.isOpen()) return;
    
    auto windowSize = window.getSize();
    
    if (currentState == GameState::GAME_SCREEN) {
        float cellSizeX = static_cast<float>(windowSize.x) / static_cast<float>(gridWidth);
        float cellSizeY = static_cast<float>(windowSize.y) / static_cast<float>(gridHeight);
        cellSize = std::min(cellSizeX, cellSizeY);
        
        float gridWidthPixels = cellSize * gridWidth;
        float gridHeightPixels = cellSize * gridHeight;
        gridOffsetX = (static_cast<float>(windowSize.x) - gridWidthPixels) / 2.0f;
        gridOffsetY = (static_cast<float>(windowSize.y) - gridHeightPixels) / 2.0f;
        
        sf::View view;
        view.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
        view.setCenter({static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f});
        window.setView(view);
    } else {
        sf::View view;
        view.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
        view.setCenter({static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f});
        window.setView(view);
        
        if (titleText.has_value()) {
            auto titleBounds = titleText->getLocalBounds();
            titleText->setPosition({
                static_cast<float>(windowSize.x) / 2.0f - titleBounds.size.x / 2.0f,
                static_cast<float>(windowSize.y) * 0.2f
            });
        }
        
        playButton.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 100.0f,
            static_cast<float>(windowSize.y) * 0.5f
        });
        
        if (playButtonText.has_value()) {
            auto playBounds = playButtonText->getLocalBounds();
            playButtonText->setPosition({
                static_cast<float>(windowSize.x) / 2.0f - playBounds.size.x / 2.0f,
                static_cast<float>(windowSize.y) * 0.5f + 10.0f
            });
        }
        
        exitButton.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 100.0f,
            static_cast<float>(windowSize.y) * 0.7f
        });
        
        if (exitButtonText.has_value()) {
            auto exitBounds = exitButtonText->getLocalBounds();
            exitButtonText->setPosition({
                static_cast<float>(windowSize.x) / 2.0f - exitBounds.size.x / 2.0f,
                static_cast<float>(windowSize.y) * 0.7f + 10.0f
            });
        }
    }
}

void SFMLUI::drawHomeScreen() {
    window.clear(sf::Color(26, 26, 26));
    
    if (titleText.has_value()) {
        window.draw(*titleText);
    }
    
    bool playHover = isMouseOver(playButton);
    bool exitHover = isMouseOver(exitButton);
    
    if (playButtonText.has_value()) {
        if (playHover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            playButtonText->setFillColor(sf::Color(0, 200, 0));
        } else if (playHover) {
            playButtonText->setFillColor(sf::Color(0, 255, 100));
        } else {
            playButtonText->setFillColor(sf::Color(0, 255, 0));
        }
        window.draw(*playButtonText);
    }
    
    if (exitButtonText.has_value()) {
        if (exitHover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            exitButtonText->setFillColor(sf::Color(200, 0, 0));
        } else if (exitHover) {
            exitButtonText->setFillColor(sf::Color(255, 100, 100));
        } else {
            exitButtonText->setFillColor(sf::Color(255, 0, 0));
        }
        window.draw(*exitButtonText);
    }
    
    window.display();
}

void SFMLUI::handleHomeScreenEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            currentState = GameState::EXIT;
            window.close();
            return;
        }
        
        if (event->is<sf::Event::Resized>()) {
            updateView();
        }
        
        if (event->is<sf::Event::MouseButtonPressed>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::MouseButtonPressed>) {
                    if (ev.button == sf::Mouse::Button::Left) {
                        if (isMouseOver(playButton)) {
                            currentState = GameState::GAME_SCREEN;
                            updateView();
                        }
                        
                        if (isMouseOver(exitButton)) {
                            currentState = GameState::EXIT;
                            window.close();
                        }
                    }
                }
            });
        }
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
        currentState = GameState::EXIT;
        window.close();
    }
}

void SFMLUI::handleGameScreenEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            currentState = GameState::EXIT;
            window.close();
            return;
        }
        
        if (event->is<sf::Event::Resized>()) {
            updateView();
        }
        
        if (event->is<sf::Event::KeyPressed>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::KeyPressed>) {
                    if (ev.scancode == sf::Keyboard::Scancode::Escape) {
                        currentState = GameState::HOME_SCREEN;
                        updateView();
                    }
                }
            });
        }
    }
}

void SFMLUI::displayGrid(const std::vector<std::vector<int>>& grid) {
    if (!window.isOpen() || currentState != GameState::GAME_SCREEN) return;

    window.clear(sf::Color(26, 26, 26));

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            sf::RectangleShape cell({cellSize - 1.0f, cellSize - 1.0f});
            cell.setPosition({gridOffsetX + x * cellSize, gridOffsetY + y * cellSize});
            
            if (grid[y][x] == 1) {
                cell.setFillColor(sf::Color::White);
            } else {
                cell.setFillColor(sf::Color::Black);
            }
            
            window.draw(cell);
        }
    }

    window.display();
}

bool SFMLUI::isWindowOpen() const {
    return window.isOpen() && currentState != GameState::EXIT;
}

bool SFMLUI::handleEvents() {
    if (!window.isOpen()) return false;
    
    switch (currentState) {
        case GameState::HOME_SCREEN:
            handleHomeScreenEvents();
            drawHomeScreen();
            break;
        case GameState::GAME_SCREEN:
            handleGameScreenEvents();
            break;
        case GameState::EXIT:
            window.close();
            return false;
    }
    
    return true;
}