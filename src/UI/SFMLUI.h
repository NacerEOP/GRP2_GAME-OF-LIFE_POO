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
    sf::Font gameFont;
    bool gameFontLoaded = false;
    
    std::optional<sf::Text> titleText;
    std::optional<sf::Text> playButtonText;
    std::optional<sf::Text> exitButtonText;
    sf::RectangleShape playButton;
    sf::RectangleShape exitButton;
    // Game screen UI
    sf::RectangleShape topBar;
    sf::RectangleShape startButton;
    sf::RectangleShape pauseButton;
    std::optional<sf::Text> startText;
    std::optional<sf::Text> pauseText;
    sf::RectangleShape inputBox;
    std::optional<sf::Text> inputText;
    bool simulationRunning = false;
    int inputValue = 0;
    bool inputActive = false;

    void updateView();
    void initializeHomeScreen();
    void initializeGameUI();
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