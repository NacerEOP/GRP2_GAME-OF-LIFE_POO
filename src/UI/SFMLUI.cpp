#include "SFMLUI.h"
#include <iostream>
#include <algorithm>

SFMLUI::SFMLUI(int width, int height, int cellSize)
    : gridWidth(width), gridHeight(height), baseCellSize(cellSize),
      cellSize(static_cast<float>(cellSize)), gridOffsetX(0), gridOffsetY(0)
{
    // Create resizable window
    window.create(
        sf::VideoMode({static_cast<unsigned int>(width * cellSize), 
                      static_cast<unsigned int>(height * cellSize)}), 
        "Game of Life - SFML 3.0.2 (Resizable)",
        sf::Style::Default
    );

    if (window.isOpen()) {
        window.setFramerateLimit(60);
        updateView(); // Set initial view
        std::cout << "Resizable SFML 3.0.2 Window created successfully!" << std::endl;
    } else {
        std::cerr << "Failed to create window with SFML 3.0.2" << std::endl;
    }
}

void SFMLUI::updateView() {
    if (!window.isOpen()) return;
    
    auto windowSize = window.getSize();
    
    // Calculate cell size that maintains aspect ratio
    float cellSizeX = static_cast<float>(windowSize.x) / static_cast<float>(gridWidth);
    float cellSizeY = static_cast<float>(windowSize.y) / static_cast<float>(gridHeight);
    cellSize = std::min(cellSizeX, cellSizeY); // Use the smaller one to fit both dimensions
    
    // Calculate the centered position for the grid
    float gridWidthPixels = cellSize * gridWidth;
    float gridHeightPixels = cellSize * gridHeight;
    gridOffsetX = (static_cast<float>(windowSize.x) - gridWidthPixels) / 2.0f;
    gridOffsetY = (static_cast<float>(windowSize.y) - gridHeightPixels) / 2.0f;
    
    // Create a view that centers the grid
    sf::View view;
    view.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    view.setCenter({static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f});
    window.setView(view);
}

void SFMLUI::displayGrid(const std::vector<std::vector<int>>& grid) {
    if (!window.isOpen()) return;

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
    return window.isOpen();
}

bool SFMLUI::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false;
        }
        
        if (event->is<sf::Event::Resized>()) {
            updateView();
        }
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
        window.close();
        return false;
    }
    
    return true;
}