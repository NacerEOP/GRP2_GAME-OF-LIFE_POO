#pragma once

#include <SFML/Graphics.hpp>

class GameService;

// Handles SFML input events and delegates to GameService
class SFMLInput {
public:
    SFMLInput() = default;
    ~SFMLInput() = default;

    // Provide UI context pointers so input can hit-test UI elements.
    void setUIContext(sf::RenderWindow* win,
                      const sf::RectangleShape* playBtn, const sf::RectangleShape* exitBtn,
                      const sf::RectangleShape* startBtn, const sf::RectangleShape* pauseBtn,
                      const sf::RectangleShape* mainMenuBtn, const sf::RectangleShape* toricBtn,
                      const sf::RectangleShape* decBtn, const sf::RectangleShape* incBtn,
                      const sf::RectangleShape* inputBx,
                      const float* gridOffX, const float* gridOffY, const float* cellSz,
                      const int* gridR, const int* gridC);

    // Process an SFML event and update GameService accordingly.
    // `isGameScreenActive` can be modified to request switch to HOME/GAME.
    // `quitRequested` will be set true if the UI requested exit (e.g., Quit button).
    bool handleEvent(GameService& service, sf::RenderWindow& window, const sf::Event& event, bool& isGameScreenActive, bool& quitRequested);
    
    // Get current input box value (for resizing grid, etc.)
    int getInputValue() const;
    void setInputValue(int val);
    void clearInputValue();
    
    // Query if input box is currently active (focus for text entry)
    bool isInputActive() const;
    void setInputActive(bool active);

private:
    int inputValue = 0;
    bool inputActive = false;
    // UI context pointers (set by SFMLUI)
    sf::RenderWindow* windowPtr = nullptr;
    const sf::RectangleShape* playButton = nullptr;
    const sf::RectangleShape* exitButton = nullptr;
    const sf::RectangleShape* startButton = nullptr;
    const sf::RectangleShape* pauseButton = nullptr;
    const sf::RectangleShape* mainMenuButton = nullptr;
    const sf::RectangleShape* toricButton = nullptr;
    const sf::RectangleShape* decButton = nullptr;
    const sf::RectangleShape* incButton = nullptr;
    const sf::RectangleShape* inputBox = nullptr;
    const float* gridOffsetX = nullptr;
    const float* gridOffsetY = nullptr;
    const float* cellSizePtr = nullptr;
    const int* gridRows = nullptr;
    const int* gridCols = nullptr;
    // Optional sound service (set by SFMLUI)
    class SoundService* soundService = nullptr;
    // hover state tracking to only play hover once per entry
    bool wasOverPlay = false;
    bool wasOverExit = false;
    bool wasOverStart = false;
    bool wasOverPause = false;
    bool wasOverMainMenu = false;
    bool wasOverToric = false;
    bool wasOverDec = false;
    bool wasOverInc = false;
    bool wasOverInputBox = false;
public:
    void setSoundService(class SoundService* s) { soundService = s; }
};
