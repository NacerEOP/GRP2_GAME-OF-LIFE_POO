#include "SFMLUI.h"
#include <iostream>
#include <algorithm>
#include <optional>
#include <filesystem>
#include <string>

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

    // Initialize game-screen UI elements (fonts may already be loaded above)
    initializeGameUI();
}

void SFMLUI::initializeGameUI() {
    namespace fs = std::filesystem;
    std::vector<fs::path> candidates;
    candidates.emplace_back("resources/fonts/Retro-font.ttf");
    candidates.emplace_back("../resources/fonts/Retro-font.ttf");
    candidates.emplace_back("../../resources/fonts/Retro-font.ttf");

    fs::path srcPath = fs::path(__FILE__);
    if (srcPath.has_parent_path()) {
        fs::path projectRoot = srcPath.parent_path().parent_path().parent_path();
        candidates.emplace_back(projectRoot / "resources" / "fonts" / "Retro-font.ttf");
    }

    bool loaded = false;
    for (const auto &p : candidates) {
        try {
            auto abs = fs::absolute(p);
            if (gameFont.openFromFile(p.string())) {
                loaded = true;
                break;
            } else {
                std::cerr << "Failed to load Retro font: " << p.string() << std::endl;
                std::cerr << "    Absolute path: " << abs.string() << std::endl;
            }
        } catch (const std::exception &ex) {
            std::cerr << "Exception while checking Retro font path: " << ex.what() << std::endl;
        }
    }
    if (!loaded) {
        std::cerr << "Retro font not loaded; game-screen will use default font." << std::endl;
    }

    // Top bar
    topBar.setSize({static_cast<float>(window.getSize().x), 60.0f});
    topBar.setFillColor(sf::Color(40, 40, 40));

    // Start/Stop button
    startStopButton.setSize({120.0f, 40.0f});
    startStopButton.setFillColor(sf::Color(80, 80, 80));

    // Start/Stop text
    if (gameFont.getInfo().family.size() > 0) {
        startStopText = sf::Text(gameFont, "Start", 20);
    } else {
        startStopText = sf::Text(font, "Start", 20);
    }
    startStopText->setFillColor(sf::Color::White);

    // Input box
    inputBox.setSize({140.0f, 36.0f});
    inputBox.setFillColor(sf::Color::Black);
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color(120, 120, 120));

    if (gameFont.getInfo().family.size() > 0) {
        inputText = sf::Text(gameFont, std::to_string(inputValue), 18);
    } else {
        inputText = sf::Text(font, std::to_string(inputValue), 18);
    }
    inputText->setFillColor(sf::Color::White);

    // ensure positions are correct
    updateView();
}

void SFMLUI::updateView() {
    if (!window.isOpen()) return;
    
    auto windowSize = window.getSize();
    
    if (currentState == GameState::GAME_SCREEN) {
        const float topBarHeight = 60.0f;
        float cellSizeX = static_cast<float>(windowSize.x) / static_cast<float>(gridWidth);
        float cellSizeY = (static_cast<float>(windowSize.y) - topBarHeight) / static_cast<float>(gridHeight);
        cellSize = std::min(cellSizeX, cellSizeY);
        
        float gridWidthPixels = cellSize * gridWidth;
        float gridHeightPixels = cellSize * gridHeight;
        gridOffsetX = (static_cast<float>(windowSize.x) - gridWidthPixels) / 2.0f;
        // place grid below the top bar and center in remaining space
        gridOffsetY = topBarHeight + (static_cast<float>(windowSize.y) - topBarHeight - gridHeightPixels) / 2.0f;
        // update top bar and controls positions
        topBar.setSize({static_cast<float>(windowSize.x), topBarHeight});
        topBar.setPosition({0.0f, 0.0f});

        // start/stop button left side
        startStopButton.setPosition({10.0f, (topBarHeight - startStopButton.getSize().y) / 2.0f});
        if (startStopText.has_value()) {
            auto tb = startStopText->getLocalBounds();
            startStopText->setPosition({startStopButton.getPosition().x + startStopButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                        startStopButton.getPosition().y + startStopButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }

        // input box to the right of button
        inputBox.setPosition({startStopButton.getPosition().x + startStopButton.getSize().x + 12.0f,
                              (topBarHeight - inputBox.getSize().y) / 2.0f});
        if (inputText.has_value()) {
            auto it = inputText->getLocalBounds();
            inputText->setPosition({inputBox.getPosition().x + 8.0f, inputBox.getPosition().y + inputBox.getSize().y / 2.0f - it.size.y / 2.0f});
        }
        
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

    // Draw top bar and controls
    window.draw(topBar);
    window.draw(startStopButton);
    if (startStopText.has_value()) window.draw(*startStopText);
    window.draw(inputBox);
    if (inputText.has_value()) window.draw(*inputText);
    
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
        // handle window close
        if (event->is<sf::Event::Closed>()) {
            currentState = GameState::EXIT;
            window.close();
            return;
        }

        // resize -> recompute layout
        if (event->is<sf::Event::Resized>()) {
            updateView();
        }

        // mouse clicks
        if (event->is<sf::Event::MouseButtonPressed>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::MouseButtonPressed>) {
                    if (ev.button == sf::Mouse::Button::Left) {
                        if (isMouseOver(startStopButton)) {
                            simulationRunning = !simulationRunning;
                            if (startStopText.has_value()) {
                                if (simulationRunning) startStopText->setString("Stop");
                                else startStopText->setString("Start");
                            }
                        }

                        if (isMouseOver(inputBox)) {
                            inputActive = true;
                        } else {
                            inputActive = false;
                        }
                    }
                }
            });
        }

        // text entry for number input
        // We handle numeric input via KeyPressed events to avoid templated
        // visitor instantiation issues with some SFML versions.

        // key presses (e.g., Escape to home)
        if (event->is<sf::Event::KeyPressed>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::KeyPressed>) {
                    // Escape -> back to home
                    if (ev.scancode == sf::Keyboard::Scancode::Escape) {
                        currentState = GameState::HOME_SCREEN;
                        updateView();
                        return;
                    }

                    // Space -> toggle simulation
                    if (ev.scancode == sf::Keyboard::Scancode::Space) {
                        simulationRunning = !simulationRunning;
                        if (startStopText.has_value()) {
                            if (simulationRunning) startStopText->setString("Stop");
                            else startStopText->setString("Start");
                        }
                        return;
                    }

                    // If input is active, accept Num0..Num9 and Backspace
                    if (inputActive) {
                        // Scancodes for numbers (Num0..Num9)
                        if (ev.scancode >= sf::Keyboard::Scancode::Num0 && ev.scancode <= sf::Keyboard::Scancode::Num9) {
                            int digit = static_cast<int>(ev.scancode) - static_cast<int>(sf::Keyboard::Scancode::Num0);
                            // append digit
                            std::string s = inputText.has_value() ? inputText->getString().toAnsiString() : std::to_string(inputValue);
                            s.push_back('0' + digit);
                            try { inputValue = std::stoi(s); } catch(...) { }
                            if (inputText.has_value()) inputText->setString(s);
                        } else if (ev.scancode == sf::Keyboard::Scancode::Backspace) {
                            std::string s = inputText.has_value() ? inputText->getString().toAnsiString() : std::to_string(inputValue);
                            if (!s.empty()) s.pop_back();
                            try { inputValue = s.empty() ? 0 : std::stoi(s); } catch(...) { inputValue = 0; }
                            if (inputText.has_value()) inputText->setString(s);
                        } else if (ev.scancode == sf::Keyboard::Scancode::Enter) {
                            inputActive = false;
                        }
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