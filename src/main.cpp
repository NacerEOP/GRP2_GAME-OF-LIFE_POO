#include "UI/ConsoleUI.h"
#include "Services/GameService.h"

int main() {
    GameService* service = createDefaultGameService();
    service->setGridSize(GridSize::SMALL); // <--- set small grid at startup
    ConsoleUI ui(*service);
    ui.run();
    delete service;
    return 0;
}
