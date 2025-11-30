#include "SFMLUI.h"
#include <iostream>
#include <algorithm>

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
    }
}

void SFMLUI::initializeHomeScreen() {
    titleBackground.setSize({400, 60});
    titleBackground.setFillColor(sf::Color::White);
    
    playButton.setSize({200, 60});
    playButton.setFillColor(sf::Color(0, 128, 0));
    
    exitButton.setSize({200, 60});
    exitButton.setFillColor(sf::Color(128, 0, 0));
    
    playText.setSize({80, 30});
    playText.setFillColor(sf::Color::White);
    
    exitText.setSize({80, 30});
    exitText.setFillColor(sf::Color::White);
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
        
        titleBackground.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 200.0f,
            static_cast<float>(windowSize.y) * 0.2f
        });
        
        playButton.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 100.0f,
            static_cast<float>(windowSize.y) * 0.5f
        });
        
        exitButton.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 100.0f,
            static_cast<float>(windowSize.y) * 0.7f
        });
        
        playText.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 40.0f,
            static_cast<float>(windowSize.y) * 0.5f + 15.0f
        });
        
        exitText.setPosition({
            static_cast<float>(windowSize.x) / 2.0f - 40.0f,
            static_cast<float>(windowSize.y) * 0.7f + 15.0f
        });
    }
}

void SFMLUI::drawHomeScreen() {
    window.clear(sf::Color(0, 0, 128));
    
    window.draw(titleBackground);
    window.draw(playButton);
    window.draw(exitButton);
    window.draw(playText);
    window.draw(exitText);
    
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
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                        
                        if (playButton.getGlobalBounds().contains(worldPos)) {
                            currentState = GameState::GAME_SCREEN;
                            updateView();
                        }
                        
                        if (exitButton.getGlobalBounds().contains(worldPos)) {
                            currentState = GameState::EXIT;
                            window.close();
                        }
                    }
                }
            });
        }
        
        if (event->is<sf::Event::MouseMoved>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::MouseMoved>) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                    
                    if (playButton.getGlobalBounds().contains(worldPos)) {
                        playButton.setFillColor(sf::Color(0, 200, 0));
                    } else {
                        playButton.setFillColor(sf::Color(0, 128, 0));
                    }
                    
                    if (exitButton.getGlobalBounds().contains(worldPos)) {
                        exitButton.setFillColor(sf::Color(200, 0, 0));
                    } else {
                        exitButton.setFillColor(sf::Color(128, 0, 0));
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

    window.clear(sf::Color::Black);

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