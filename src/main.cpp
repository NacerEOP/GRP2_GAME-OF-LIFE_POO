#include "UI/ConsoleUI.h"
#include "Services/GameService.h"

int main() {
    GameService* service = new GameService();
    service->setGridSize(GridSize::NORMAL); // set normal grid at startup
    ConsoleUI ui(*service);
    ui.run();
    delete service;
    return 0;
}
