#include "SFMLInput.h"
#include "../Services/GameService.h"
#include <iostream>
#include <optional>

void SFMLInput::setUIContext(sf::RenderWindow* win,
                      const sf::RectangleShape* playBtn, const sf::RectangleShape* exitBtn,
                      const sf::RectangleShape* startBtn, const sf::RectangleShape* pauseBtn,
                      const sf::RectangleShape* mainMenuBtn, const sf::RectangleShape* toricBtn,
                      const sf::RectangleShape* decBtn, const sf::RectangleShape* incBtn,
                      const sf::RectangleShape* inputBx,
                      const float* gridOffX, const float* gridOffY, const float* cellSz,
                      const int* gridR, const int* gridC) {
    windowPtr = win;
    playButton = playBtn; exitButton = exitBtn;
    startButton = startBtn; pauseButton = pauseBtn;
    mainMenuButton = mainMenuBtn; toricButton = toricBtn;
    decButton = decBtn; incButton = incBtn; inputBox = inputBx;
    gridOffsetX = gridOffX; gridOffsetY = gridOffY; cellSizePtr = cellSz;
    gridRows = gridR; gridCols = gridC;
}

bool SFMLInput::handleEvent(GameService& service, sf::RenderWindow& window, const sf::Event& event, bool& isGameScreenActive, bool& quitRequested) {
    // helper: hit test a rectangle with current mouse position
    auto isMouseOverRect = [&](const sf::RectangleShape* rect) -> bool {
        if (!rect || !windowPtr) return false;
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::Vector2f wp = window.mapPixelToCoords(mp);
        return rect->getGlobalBounds().contains(wp);
    };

    // Handle keyboard shortcuts and input
    if (event.is<sf::Event::KeyPressed>()) {
        event.visit([this, &service, &isGameScreenActive, &quitRequested](auto&& ev) {
            if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::KeyPressed>) {
                if (ev.scancode == sf::Keyboard::Scan::Escape) {
                    // Go back to home screen
                    isGameScreenActive = false;
                    return;
                }
                
                if (ev.scancode == sf::Keyboard::Scan::Space) {
                    // Toggle start/pause
                    if (service.isRunning()) {
                        service.pause();
                    } else {
                        service.start();
                    }
                    return;
                }
                
                if (ev.scancode == sf::Keyboard::Scan::R) {
                    // Reset simulation
                    service.reset();
                    return;
                }
                
                if (ev.scancode == sf::Keyboard::Scan::S) {
                    // Single step
                    service.step();
                    return;
                }
                
                // Presets: 0-9 keys to load presets
                if (ev.scancode >= sf::Keyboard::Scan::Num0 && ev.scancode <= sf::Keyboard::Scan::Num9) {
                    int slot = static_cast<int>(ev.scancode) - static_cast<int>(sf::Keyboard::Scan::Num0);
                    service.loadPreset(slot);
                    return;
                }
                
                // Input box: backspace
                if (inputActive && ev.scancode == sf::Keyboard::Scan::Backspace) {
                    if (inputValue >= 10) {
                        inputValue /= 10;
                    } else {
                        inputValue = 0;
                    }
                    return;
                }
                
                // Input box: enter
                if (inputActive && ev.scancode == sf::Keyboard::Scan::Enter) {
                    // Apply input (e.g., set grid dimensions)
                    if (inputValue > 0) {
                        service.setGridDimensions(inputValue, inputValue);
                    }
                    inputActive = false;
                    return;
                }
                
                // +/- for speed control
                if (ev.scancode == sf::Keyboard::Scan::Equal) {
                    int ms = service.getTickMs();
                    if (ms > 10) service.setTickMs(ms - 10);
                    return;
                }
                if (ev.scancode == sf::Keyboard::Scan::Hyphen) {
                    int ms = service.getTickMs();
                    service.setTickMs(ms + 10);
                    return;
                }
                
                // T for toric toggle
                if (ev.scancode == sf::Keyboard::Scan::T) {
                    service.setToric(!service.isToric());
                    return;
                }
            }
        });
    }
    
    // Handle text input (numeric entry)
    // Handle text input (numeric entry)
    if (event.is<sf::Event::TextEntered>()) {
        event.visit([this](auto&& ev) {
            if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::TextEntered>) {
                if (!inputActive) return;
                uint32_t unicode = ev.unicode;
                // Accept ASCII digits
                if (unicode >= '0' && unicode <= '9') {
                    inputValue = inputValue * 10 + (unicode - '0');
                    // Cap at reasonable max
                    if (inputValue > 500) inputValue = 500;
                }
            }
        });
    }
    
    // Mouse handling: clicks and grid toggles
    if (event.is<sf::Event::MouseButtonPressed>()) {
        event.visit([&](auto&& ev) {
            if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::MouseButtonPressed>) {
                if (ev.button == sf::Mouse::Button::Left) {
                    // Home screen: play/quit
                    if (isMouseOverRect(playButton)) { isGameScreenActive = true; return; }
                    if (isMouseOverRect(exitButton)) { quitRequested = true; return; }

                    // Game-screen controls
                    if (isMouseOverRect(startButton)) { service.start(); return; }
                    if (isMouseOverRect(pauseButton)) { service.pause(); return; }
                    if (isMouseOverRect(mainMenuButton)) { isGameScreenActive = false; return; }
                    if (isMouseOverRect(toricButton)) { service.setToric(!service.isToric()); return; }
                    if (isMouseOverRect(decButton)) { int ms = service.getTickMs(); service.setTickMs(std::min(2000, ms + 50)); return; }
                    if (isMouseOverRect(incButton)) { int ms = service.getTickMs(); service.setTickMs(std::max(10, ms - 50)); return; }

                    if (isMouseOverRect(inputBox)) { setInputActive(true); return; } else { setInputActive(false); }

                    // Click on grid to toggle cells
                    if (gridOffsetX && gridOffsetY && cellSizePtr && gridRows && gridCols) {
                        sf::Vector2i mp = sf::Mouse::getPosition(window);
                        sf::Vector2f wp = window.mapPixelToCoords(mp);
                        float gx = *gridOffsetX; float gy = *gridOffsetY; float cs = *cellSizePtr;
                        int rows = *gridRows; int cols = *gridCols;
                        float gridWidthPixels = cs * cols;
                        float gridHeightPixels = cs * rows;
                        if (wp.x >= gx && wp.x < gx + gridWidthPixels && wp.y >= gy && wp.y < gy + gridHeightPixels) {
                            int col = static_cast<int>((wp.x - gx) / cs);
                            int row = static_cast<int>((wp.y - gy) / cs);
                            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                                bool current = service.getCell(row, col);
                                service.setCell(row, col, !current);
                            }
                        }
                    }
                } else if (ev.button == sf::Mouse::Button::Right) {
                    // Right click toggles obstacle flag on that cell
                    if (gridOffsetX && gridOffsetY && cellSizePtr && gridRows && gridCols) {
                        sf::Vector2i mp = sf::Mouse::getPosition(window);
                        sf::Vector2f wp = window.mapPixelToCoords(mp);
                        float gx = *gridOffsetX; float gy = *gridOffsetY; float cs = *cellSizePtr;
                        int rows = *gridRows; int cols = *gridCols;
                        float gridWidthPixels = cs * cols;
                        float gridHeightPixels = cs * rows;
                        if (wp.x >= gx && wp.x < gx + gridWidthPixels && wp.y >= gy && wp.y < gy + gridHeightPixels) {
                            int col = static_cast<int>((wp.x - gx) / cs);
                            int row = static_cast<int>((wp.y - gy) / cs);
                            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                                bool obs = service.isObstacle(row, col);
                                service.setObstacle(row, col, !obs);
                                if (!obs) service.setCell(row, col, false);
                            }
                        }
                    }
                }
            }
        });
    }

    return true;
}

int SFMLInput::getInputValue() const { return inputValue; }
void SFMLInput::setInputValue(int val) { inputValue = val; }
void SFMLInput::clearInputValue() { inputValue = 0; }

bool SFMLInput::isInputActive() const { return inputActive; }
<<<<<<< HEAD
void SFMLInput::setInputActive(bool active) { inputActive = active; }

=======
void SFMLInput::setInputActive(bool active) { inputActive = active; }
>>>>>>> 7148f0781f908da53d947462047cfc2436034aad
