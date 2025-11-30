#include "UI/SFMLUI.h"
#include <vector>
#include <thread>
#include <chrono>

int main()
{
    #ifdef _WIN32
    _putenv("SFML_OPENGL_ES=1");
    #endif

    SFMLUI ui(50, 30, 15);

    if (!ui.isWindowOpen()) {
        return -1;
    }

    std::vector<std::vector<int>> grid(30, std::vector<int>(50, 0));
    
    grid[10][10] = 1;
    grid[11][11] = 1;
    grid[12][9] = 1;
    grid[12][10] = 1;
    grid[12][11] = 1;

    while (ui.isWindowOpen()) {
        if (!ui.handleEvents()) {
            break;
        }
        
        if (ui.getCurrentState() == GameState::GAME_SCREEN) {
            ui.displayGrid(grid);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}