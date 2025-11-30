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
    sf::Font font;
    
    std::optional<sf::Text> titleText;
    std::optional<sf::Text> playButtonText;
    std::optional<sf::Text> exitButtonText;
    sf::RectangleShape playButton;
    sf::RectangleShape exitButton;

    void updateView();
    void initializeHomeScreen();
    void handleHomeScreenEvents();
    void drawHomeScreen();
    void handleGameScreenEvents();
    bool isMouseOver(const sf::RectangleShape& button);

public:
    SFMLUI(int width, int height, int cellSize);
    ~SFMLUI() = default;

    void displayGrid(const std::vector<std::vector<int>>& grid);
    bool isWindowOpen() const;
    bool handleEvents();
    GameState getCurrentState() const { return currentState; }
};

#endif