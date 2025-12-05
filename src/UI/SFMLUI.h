#ifndef SFMLUI_H
#define SFMLUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "../Input/SFMLInput.h"
#include <optional>
#include "../Core/Grid.h"
#include <chrono>

enum class GameState {
    HOME_SCREEN,
    GAME_SCREEN,
    EXIT
};

// Forward declaration
class GameService;

class SFMLUI {
private:
    GameService& service;  // Reference to the service hub
    SFMLInput inputHandler;  // Delegates input to GameService
    
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
    sf::RectangleShape mainMenuButton;
    std::optional<sf::Text> startText;
    std::optional<sf::Text> pauseText;
    std::optional<sf::Text> mainMenuText;
    sf::RectangleShape inputBox;
    std::optional<sf::Text> inputText;
    int inputValue = 0;
    bool inputActive = false;
    // Top bar extras: tick display and toric toggle, and inc/dec buttons
    std::optional<sf::Text> tickText;
    sf::RectangleShape toricToggleButton;
    std::optional<sf::Text> toricText;
    sf::RectangleShape decButton;
    sf::RectangleShape incButton;
    std::optional<sf::Text> decText;
    std::optional<sf::Text> incText;
    // (menu simulation removed — main menu uses static UI elements only)

    void updateView();
    void initializeHomeScreen();
    void initializeGameUI();
    void handleHomeScreenEvents();
    void drawHomeScreen();
    void handleGameScreenEvents();
    void drawGameScreen();  // Render game screen from GameService data
    bool isMouseOver(const sf::RectangleShape& button);
    void syncGridWithService();  // Pull grid dimensions/data from GameService
    // menu simulation removed

public:
    SFMLUI(GameService& service);
    ~SFMLUI() = default;

    bool isWindowOpen() const;
    bool handleEvents();
    void render();  // Render current state based on GameService
    GameState getCurrentState() const { return currentState; }
};

#endif