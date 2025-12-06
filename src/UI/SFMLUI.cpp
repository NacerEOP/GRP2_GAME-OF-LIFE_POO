#include "SFMLUI.h"
#include "../Services/GameService.h"
#include <iostream>
#include <algorithm>
#include <optional>
#include <filesystem>
#include <string>
#include <chrono>

SFMLUI::SFMLUI(GameService& svc)
    : service(svc), gridWidth(50), gridHeight(30), baseCellSize(15),
      cellSize(static_cast<float>(baseCellSize)), gridOffsetX(0), gridOffsetY(0),
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
    
    // Sync grid dimensions from GameService
    syncGridWithService();
}

void SFMLUI::syncGridWithService() {
    int newCols = service.getCols();
    int newRows = service.getRows();
    if (newCols != gridWidth || newRows != gridHeight) {
        gridWidth = newCols;
        gridHeight = newRows;
        // Recompute layout when grid size changes
        updateView();
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

    // Provide input handler with UI context for home screen buttons
    inputHandler.setUIContext(&window, &playButton, &exitButton, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    // provide sound service to input handler
    inputHandler.setSoundService(&soundService);

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

    // Top bar (dark theme) — size will be set in updateView() to match grid width
    topBar.setSize({0.0f, 64.0f});
    topBar.setFillColor(sf::Color(26, 26, 26));

    // Tick / delay text
    if (gameFont.getInfo().family.size() > 0) {
        tickText = sf::Text(gameFont, std::to_string(service.getTickMs()) + " ms", 18);
    } else {
        tickText = sf::Text(font, std::to_string(service.getTickMs()) + " ms", 18);
    }
    tickText->setFillColor(sf::Color::White);

    // Toric toggle button
    toricToggleButton.setSize({120.0f, 36.0f});
    toricToggleButton.setFillColor(sf::Color(40, 40, 40));
    toricToggleButton.setOutlineColor(sf::Color(120, 120, 120));
    toricToggleButton.setOutlineThickness(1.0f);
    if (gameFont.getInfo().family.size() > 0) {
        toricText = sf::Text(gameFont, service.isToric() ? "Toric" : "!Toric", 16);
    } else {
        toricText = sf::Text(font, service.isToric() ? "Toric" : "!Toric", 16);
    }
    toricText->setFillColor(sf::Color::White);

    // Decrease / Increase buttons for tick
    decButton.setSize({36.0f, 36.0f});
    decButton.setFillColor(sf::Color(40, 40, 40));
    decButton.setOutlineColor(sf::Color(120, 120, 120));
    decButton.setOutlineThickness(1.0f);
    incButton.setSize({36.0f, 36.0f});
    incButton.setFillColor(sf::Color(40, 40, 40));
    incButton.setOutlineColor(sf::Color(120, 120, 120));
    incButton.setOutlineThickness(1.0f);
    if (gameFont.getInfo().family.size() > 0) {
        decText = sf::Text(gameFont, "-", 18);
        incText = sf::Text(gameFont, "+", 18);
    } else {
        decText = sf::Text(font, "-", 18);
        incText = sf::Text(font, "+", 18);
    }
    decText->setFillColor(sf::Color::White);
    incText->setFillColor(sf::Color::White);

    // Start and Pause buttons (dark background, white text)
    startButton.setSize({120.0f, 44.0f});
    startButton.setFillColor(sf::Color(26, 26, 26));
    startButton.setOutlineColor(sf::Color(120, 120, 120));
    startButton.setOutlineThickness(1.0f);
    pauseButton.setSize({120.0f, 44.0f});
    pauseButton.setFillColor(sf::Color(26, 26, 26));
    pauseButton.setOutlineColor(sf::Color(120, 120, 120));
    pauseButton.setOutlineThickness(1.0f);

    // Start/Pause texts
    if (gameFont.getInfo().family.size() > 0) {
        startText = sf::Text(gameFont, "Start", 20);
        pauseText = sf::Text(gameFont, "Pause", 20);
    } else {
        startText = sf::Text(font, "Start", 20);
        pauseText = sf::Text(font, "Pause", 20);
    }
    // Button text should be white on dark bg
    startText->setFillColor(sf::Color::White);
    pauseText->setFillColor(sf::Color::White);

    // Input box
    inputBox.setSize({160.0f, 40.0f});
    inputBox.setFillColor(sf::Color(26, 26, 26));
    inputBox.setOutlineThickness(1.5f);
    inputBox.setOutlineColor(sf::Color(120, 120, 120));

    if (gameFont.getInfo().family.size() > 0) {
        inputText = sf::Text(gameFont, std::to_string(inputValue), 18);
    } else {
        inputText = sf::Text(font, std::to_string(inputValue), 18);
    }
    inputText->setFillColor(sf::Color::White);

    // Main menu button (right side of top bar)
    mainMenuButton.setSize({120.0f, 36.0f});
    mainMenuButton.setFillColor(sf::Color(26, 26, 26));
    mainMenuButton.setOutlineColor(sf::Color(120, 120, 120));
    mainMenuButton.setOutlineThickness(1.0f);
    if (gameFont.getInfo().family.size() > 0) {
        mainMenuText = sf::Text(gameFont, "Menu", 16);
    } else {
        mainMenuText = sf::Text(font, "Menu", 16);
    }
    mainMenuText->setFillColor(sf::Color::White);

    // ensure positions are correct
    updateView();

    // Provide full UI context to the input handler (game screen)
    inputHandler.setUIContext(&window, &playButton, &exitButton, &startButton, &pauseButton, &mainMenuButton, &toricToggleButton, &decButton, &incButton, &inputBox, &gridOffsetX, &gridOffsetY, &cellSize, &gridHeight, &gridWidth);
    // provide sound service to input handler
    inputHandler.setSoundService(&soundService);
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
        // update top bar and controls positions: top bar aligns horizontally with grid
        topBar.setSize({gridWidthPixels, topBarHeight});
        topBar.setPosition({gridOffsetX, 0.0f});

        // start and pause buttons on the left inside the top bar; size scales with grid width
        float btnW = std::min(140.0f, gridWidthPixels * 0.11f);
        float btnH = std::max(28.0f, topBarHeight * 0.6f);
        startButton.setSize({btnW, btnH});
        pauseButton.setSize({btnW, btnH});
        startButton.setPosition({gridOffsetX + 8.0f, (topBarHeight - startButton.getSize().y) / 2.0f});
        pauseButton.setPosition({startButton.getPosition().x + startButton.getSize().x + 8.0f,
                     (topBarHeight - pauseButton.getSize().y) / 2.0f});
        // update text sizes to match
        unsigned int charSize = static_cast<unsigned int>(std::max(12.0f, btnH * 0.55f));
        if (startText.has_value()) {
            startText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, "Start", charSize);
            auto tb = startText->getLocalBounds();
            startText->setFillColor(sf::Color::White);
            startText->setPosition({startButton.getPosition().x + startButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                    startButton.getPosition().y + startButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }
        if (pauseText.has_value()) {
            pauseText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, "Pause", charSize);
            auto tb = pauseText->getLocalBounds();
            pauseText->setFillColor(sf::Color::White);
            pauseText->setPosition({pauseButton.getPosition().x + pauseButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                    pauseButton.getPosition().y + pauseButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }

        // input box to the right of pause button (inside top bar)
        float inputW = std::min(220.0f, gridWidthPixels * 0.18f);
        inputBox.setSize({inputW, inputBox.getSize().y});
        inputBox.setPosition({pauseButton.getPosition().x + pauseButton.getSize().x + 12.0f,
                              (topBarHeight - inputBox.getSize().y) / 2.0f});
        if (inputText.has_value()) {
            inputText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, std::to_string(inputValue), charSize - 2);
            inputText->setFillColor(sf::Color::White);
            auto it = inputText->getLocalBounds();
            inputText->setPosition({inputBox.getPosition().x + 8.0f, inputBox.getPosition().y + inputBox.getSize().y / 2.0f - it.size.y / 2.0f});
        }

        // main menu button: right aligned inside the top bar
        float mmw = std::min(140.0f, gridWidthPixels * 0.11f);
        mainMenuButton.setSize({mmw, mainMenuButton.getSize().y});
        mainMenuButton.setPosition({gridOffsetX + gridWidthPixels - mmw - 8.0f, (topBarHeight - mainMenuButton.getSize().y) / 2.0f});
        if (mainMenuText.has_value()) {
            mainMenuText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, "Menu", static_cast<unsigned int>(std::max(12.0f, btnH * 0.4f)));
            mainMenuText->setFillColor(sf::Color::White);
            auto tb = mainMenuText->getLocalBounds();
            mainMenuText->setPosition({mainMenuButton.getPosition().x + mainMenuButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                       mainMenuButton.getPosition().y + mainMenuButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }

        // Tick display and toric toggle positions (on top bar, right of input box)
        float controlsX = inputBox.getPosition().x + inputBox.getSize().x + 12.0f;
        // dec/inc buttons
        decButton.setSize({btnH, btnH});
        incButton.setSize({btnH, btnH});
        decButton.setPosition({controlsX, (topBarHeight - btnH) / 2.0f});
        incButton.setPosition({decButton.getPosition().x + decButton.getSize().x + 6.0f, (topBarHeight - btnH) / 2.0f});
        // tick text between buttons and toric
        if (tickText.has_value()) {
            tickText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, std::to_string(service.getTickMs()) + " ms", static_cast<unsigned int>(std::max(12.0f, btnH * 0.45f)));
            tickText->setFillColor(sf::Color::White);
            tickText->setPosition({incButton.getPosition().x + incButton.getSize().x + 8.0f, (topBarHeight - btnH) / 2.0f});
        }
        // toric toggle to the right
        toricToggleButton.setSize({btnW * 0.9f, btnH});
        toricToggleButton.setPosition({gridOffsetX + gridWidthPixels - toricToggleButton.getSize().x - mmw - 18.0f, (topBarHeight - btnH) / 2.0f});
        if (toricText.has_value()) {
            toricText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, service.isToric() ? "Toric" : "!Toric", static_cast<unsigned int>(std::max(12.0f, btnH * 0.45f)));
            toricText->setFillColor(sf::Color::White);
            auto tb = toricText->getLocalBounds();
            toricText->setPosition({toricToggleButton.getPosition().x + toricToggleButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                    toricToggleButton.getPosition().y + toricToggleButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }
        if (decText.has_value()) {
            decText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, "-", static_cast<unsigned int>(std::max(12.0f, btnH * 0.5f)));
            decText->setFillColor(sf::Color::White);
            auto tb = decText->getLocalBounds();
            decText->setPosition({decButton.getPosition().x + decButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                  decButton.getPosition().y + decButton.getSize().y / 2.0f - tb.size.y / 2.0f});
        }
        if (incText.has_value()) {
            incText = sf::Text(gameFont.getInfo().family.size() ? gameFont : font, "+", static_cast<unsigned int>(std::max(12.0f, btnH * 0.5f)));
            incText->setFillColor(sf::Color::White);
            auto tb = incText->getLocalBounds();
            incText->setPosition({incButton.getPosition().x + incButton.getSize().x / 2.0f - tb.size.x / 2.0f,
                                  incButton.getPosition().y + incButton.getSize().y / 2.0f - tb.size.y / 2.0f});
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
        // menu layout removed
    }
}

// menu simulation removed; restore simple home-screen drawing
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

        bool isGame = (currentState == GameState::GAME_SCREEN);
        bool quitRequested = false;
        inputHandler.handleEvent(service, window, *event, isGame, quitRequested);

        if (quitRequested) {
            currentState = GameState::EXIT;
            window.close();
            return;
        }

        if (isGame && currentState != GameState::GAME_SCREEN) {
            // switched to game screen (e.g., Play pressed)
            currentState = GameState::GAME_SCREEN;
            initializeGameUI();
            updateView();
        } else if (!isGame) {
            // ensure home screen layout is recomputed (fix centering after
            // returning from game screen where window may have been resized)
            currentState = GameState::HOME_SCREEN;
            updateView();
        }
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

        // Delegate input handling to SFMLInput (centralized)
        bool isGameScreenActive = (currentState == GameState::GAME_SCREEN);
        bool quitRequested = false;
        inputHandler.handleEvent(service, window, *event, isGameScreenActive, quitRequested);
        if (quitRequested) {
            currentState = GameState::EXIT;
            window.close();
            return;
        }
        if (!isGameScreenActive) {
            currentState = GameState::HOME_SCREEN;
            // recompute positions for the home screen immediately so texts/buttons
            // are centered without needing another resize
            updateView();
        }

        // Update input text display
        if (inputText.has_value()) {
            inputText->setString(std::to_string(inputHandler.getInputValue()));
        }
    }
}

void SFMLUI::drawGameScreen() {
    window.clear(sf::Color(26, 26, 26));
    
    // Draw top bar and controls
    window.draw(topBar);
    window.draw(startButton);
    window.draw(pauseButton);
    if (startText.has_value()) window.draw(*startText);
    if (pauseText.has_value()) window.draw(*pauseText);
    window.draw(inputBox);
    if (inputText.has_value()) window.draw(*inputText);
    // draw dec/inc and tick text
    window.draw(decButton);
    window.draw(incButton);
    if (decText.has_value()) window.draw(*decText);
    if (incText.has_value()) window.draw(*incText);
    if (tickText.has_value()) window.draw(*tickText);
    // toric toggle
    window.draw(mainMenuButton);
    if (mainMenuText.has_value()) window.draw(*mainMenuText);
    window.draw(toricToggleButton);
    if (toricText.has_value()) window.draw(*toricText);

    // Draw grid from GameService
    syncGridWithService();
    int rows = service.getRows();
    int cols = service.getCols();

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            sf::RectangleShape cell({cellSize - 1.0f, cellSize - 1.0f});
            cell.setPosition({gridOffsetX + c * cellSize, gridOffsetY + r * cellSize});

            if (service.getCell(r, c)) {
                cell.setFillColor(sf::Color::White);
            } else {
                cell.setFillColor(sf::Color::Black);
            }

            window.draw(cell);
        }
    }

    window.display();
}



void SFMLUI::render() {
    switch (currentState) {
        case GameState::HOME_SCREEN:
            handleHomeScreenEvents();
            drawHomeScreen();
            break;
        case GameState::GAME_SCREEN:
            handleGameScreenEvents();
            drawGameScreen();
            break;
        case GameState::EXIT:
            window.close();
            break;
    }
}




bool SFMLUI::isWindowOpen() const {
    return window.isOpen() && currentState != GameState::EXIT;
}

bool SFMLUI::handleEvents() {
    if (!window.isOpen()) return false;
    
    render();
    
    return true;
}