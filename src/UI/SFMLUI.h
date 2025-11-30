#ifndef SFMLUI_H
#define SFMLUI_H

#include <SFML/Graphics.hpp>
#include <vector>

enum class GameState {
    HOME_SCREEN,
    GAME_SCREEN,
    EXIT
};

class SFMLUI {
private:
    int gridWidth;
    int gridHeight;
    int baseCellSize;
    sf::RenderWindow window;
    float cellSize;
    float gridOffsetX;
    float gridOffsetY;
    
    GameState currentState;
    
    sf::RectangleShape titleBackground;
    sf::RectangleShape playButton;
    sf::RectangleShape exitButton;
    sf::RectangleShape playText;
    sf::RectangleShape exitText;

    void updateView();
    void initializeHomeScreen();
    void handleHomeScreenEvents();
    void drawHomeScreen();
    void handleGameScreenEvents();

public:
    SFMLUI(int width, int height, int cellSize);
    ~SFMLUI() = default;

    void displayGrid(const std::vector<std::vector<int>>& grid);
    bool isWindowOpen() const;
    bool handleEvents();
    GameState getCurrentState() const { return currentState; }
};

#endif