#include "UI/ConsoleUI.h"
#include "Services/GameService.h"

int main() {
	GameService* service = createDefaultGameService();
	ConsoleUI ui(*service);
	ui.run();
	delete service;
	return 0;
}
