#include "UI/SFMLUI.h"
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

int main()
{
    // Try to set environment variable for ANGLE
    #ifdef _WIN32
    _putenv("SFML_OPENGL_ES=1");
    #endif

    // Create a sample grid
    std::vector<std::vector<int>> grid(30, std::vector<int>(50, 0));
    
    // Initialize some cells for testing (glider pattern)
    grid[10][10] = 1;
    grid[11][11] = 1;
    grid[12][9] = 1;
    grid[12][10] = 1;
    grid[12][11] = 1;

    SFMLUI ui(50, 30, 15);

    if (!ui.isWindowOpen()) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }

    std::cout << "SFML 3.0.2 Game of Life running! Close window to exit." << std::endl;

    // Main loop
    while (ui.isWindowOpen()) {
        // Handle events - if handleEvents returns false, break the loop
        if (!ui.handleEvents()) {
            break;
        }
        
        // Display the current grid state
        ui.displayGrid(grid);
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Application exited normally." << std::endl;
    return 0;
}