#include "UI/ConsoleUI.h"
#include "Services/GameService.h"
#include <iostream>
#include <limits>

#include "UI/SFMLUI.h"
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

        GameService* service = new GameService();
        service->setGridSize(GridSize::NORMAL);
        SFMLUI ui(*service);

        if (!ui.isWindowOpen()) {
            delete service;
            return -1;
        }

        while (ui.isWindowOpen()) {
            if (!ui.handleEvents()) {
                break;
            }
            
            // Step the simulation
            if (service->isRunning()) {
                service->step();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(service->getTickMs()));
        }
        
        delete service;
    }
    
                                                         


    return 0;
}
