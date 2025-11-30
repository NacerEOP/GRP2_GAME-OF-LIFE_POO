#ifndef SFMLUI_H
#define SFMLUI_H

#include <SFML/Graphics.hpp>
#include <vector>

class SFMLUI {
private:
    int gridWidth;
    int gridHeight;
    int baseCellSize;
    sf::RenderWindow window;
    float cellSize;
    float gridOffsetX;
    float gridOffsetY;

    void updateView();

public:
    SFMLUI(int width, int height, int cellSize);
    ~SFMLUI() = default;

    void displayGrid(const std::vector<std::vector<int>>& grid);
    bool isWindowOpen() const;
    bool handleEvents();
};

#endif