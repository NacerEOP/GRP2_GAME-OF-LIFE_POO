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

    // Do NOT initialize game UI here — initialize it when entering GAME_SCREEN
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

    // Top bar (bright color for visibility)
    topBar.setSize({static_cast<float>(window.getSize().x), 64.0f});
    topBar.setFillColor(sf::Color(50, 100, 160));

    // Start and Pause buttons (more visible)
    startButton.setSize({120.0f, 44.0f});
    startButton.setFillColor(sf::Color(60, 180, 75));
    startButton.setOutlineColor(sf::Color::Black);
    startButton.setOutlineThickness(2.0f);
    pauseButton.setSize({120.0f, 44.0f});
    pauseButton.setFillColor(sf::Color(220, 70, 70));
    pauseButton.setOutlineColor(sf::Color::Black);
    pauseButton.setOutlineThickness(2.0f);

    // Start/Pause texts
    if (gameFont.getInfo().family.size() > 0) {
        startText = sf::Text(gameFont, "Start", 20);
        pauseText = sf::Text(gameFont, "Pause", 20);
    } else {
        startText = sf::Text(font, "Start", 20);
        pauseText = sf::Text(font, "Pause", 20);
    }
    startText->setFillColor(sf::Color::Black);
    pauseText->setFillColor(sf::Color::Black);

    // Input box
    inputBox.setSize({160.0f, 40.0f});
    inputBox.setFillColor(sf::Color(240, 240, 240));
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color::Black);

    if (gameFont.getInfo().family.size() > 0) {
        inputText = sf::Text(gameFont, std::to_string(inputValue), 18);
    } else {
        inputText = sf::Text(font, std::to_string(inputValue), 18);
    }
    inputText->setFillColor(sf::Color::Black);

    // ensure positions are correct
    updateView();
}

void SFMLUI::updateView() {
    if (!window.isOpen()) return;
    
    auto windowSize = window.getSize();
    
    if (currentState == GameState::GAME_SCREEN) {
        const float topBarHeight = 64.0f;
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

        // start and pause buttons on the left
        startButton.setPosition({10.0f, (topBarHeight - startButton.getSize().y) / 2.0f});
        pauseButton.setPosition({startButton.getPosition().x + startButton.getSize().x + 8.0f,
                                 (topBarHeight - pauseButton.getSize().y) / 2.0f});
        if (startText.has_value()) {
            auto tb = startText->getLocalBounds();
            startText->setPosition({startButton.getPosition().x + startButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                    startButton.getPosition().y + startButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }
        if (pauseText.has_value()) {
            auto tb = pauseText->getLocalBounds();
            pauseText->setPosition({pauseButton.getPosition().x + pauseButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                    pauseButton.getPosition().y + pauseButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }

        // input box to the right of pause button
        inputBox.setPosition({pauseButton.getPosition().x + pauseButton.getSize().x + 12.0f,
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

    // Home screen draws only its own elements
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
                            // initialize game UI now (only when entering game screen)
                            initializeGameUI();
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
                        if (isMouseOver(startButton)) {
                            simulationRunning = true;
                            // update visuals
                            startButton.setFillColor(sf::Color(120, 200, 120));
                            pauseButton.setFillColor(sf::Color(160, 80, 80));
                        }
                        if (isMouseOver(pauseButton)) {
                            simulationRunning = false;
                            startButton.setFillColor(sf::Color(80, 160, 80));
                            pauseButton.setFillColor(sf::Color(200, 120, 120));
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

        // Text input (Unicode-aware): use TextEntered so users can type numbers
        if (event->is<sf::Event::TextEntered>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::TextEntered>) {
                    if (!inputActive) return;
                    uint32_t unicode = ev.unicode;
                    // accept ASCII digits only for now
                    if (unicode >= '0' && unicode <= '9') {
                        std::string s = inputText.has_value() ? inputText->getString().toAnsiString() : std::to_string(inputValue);
                        s.push_back(static_cast<char>(unicode));
                        try { inputValue = std::stoi(s); } catch(...) { }
                        if (inputText.has_value()) inputText->setString(s);
                    }
                }
            });
        }

        // key presses (e.g., Escape to home, Backspace, Enter)
        if (event->is<sf::Event::KeyPressed>()) {
            event->visit([this](auto&& ev) {
                if constexpr (std::is_same_v<std::decay_t<decltype(ev)>, sf::Event::KeyPressed>) {
                    // Escape -> back to home
                    if (ev.scancode == sf::Keyboard::Scancode::Escape) {
                        currentState = GameState::HOME_SCREEN;
                        updateView();
                        return;
                    }

                    // Space -> toggle simulation and update visuals
                    if (ev.scancode == sf::Keyboard::Scancode::Space) {
                        simulationRunning = !simulationRunning;
                        if (simulationRunning) {
                            startButton.setFillColor(sf::Color(120, 200, 120));
                            pauseButton.setFillColor(sf::Color(160, 80, 80));
                        } else {
                            startButton.setFillColor(sf::Color(80, 160, 80));
                            pauseButton.setFillColor(sf::Color(200, 120, 120));
                        }
                        return;
                    }

                    // Backspace / Enter handling when input active
                    if (inputActive) {
                        if (ev.scancode == sf::Keyboard::Scancode::Backspace) {
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
    // Draw top bar and controls (if initialized)
    window.draw(topBar);
    window.draw(startButton);
    window.draw(pauseButton);
    if (startText.has_value()) window.draw(*startText);
    if (pauseText.has_value()) window.draw(*pauseText);
    window.draw(inputBox);
    if (inputText.has_value()) window.draw(*inputText);

    // Draw grid below the top bar
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