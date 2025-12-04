#include "UI/SFMLUI.h"
#include "UI/ConsoleUI.h"
#include "Services/GameService.h"
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <chrono>

int main() {
   int choix;
    
    std::cout << "Entrez 1 ou 2: ";
    
    // Boucle jusqu'à obtenir une entrée valide
    while (!(std::cin >> choix) || (choix != 1 && choix != 2)) {
        std::cout << "Entrée invalide! Veuillez entrer 1 ou 2: ";
        
        // Réinitialiser l'état de cin
        std::cin.clear();
        
        // Ignorer les caractères restants dans le buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::cout << "Vous avez choisi: " << choix << std::endl;
      std::cin.clear();

    if (choix==1)
    {
        GameService* service = new GameService();
    service->setGridSize(GridSize::NORMAL);   // set normal grid at startup 
    ConsoleUI ui(*service);
    ui.run();
    delete service;
    }
    else if (choix==2)
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

    
}
    return 0;
    }
    
                                                         


    

