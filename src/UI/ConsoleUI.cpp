#include "ConsoleUI.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib> // system("cls")
#include "../Input/ConsoleInput.h"

ConsoleUI::ConsoleUI(GameService& svc) : service(svc) {}

ConsoleUI::~ConsoleUI() = default;

void ConsoleUI::run() {
	bool quit = false;
	using clock = std::chrono::steady_clock;
	auto last = clock::now();

	//drawHelp();
	render();

	while (!quit) {
		if (service.isRunning()) {
			auto now = clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
			if (elapsed >= service.getTickMs()) {
				service.step();
				render();
				last = now;
			}
		}

		// Delegate input handling to ConsoleInput
		static ConsoleInput inputHandler;
		if (inputHandler.handleInput(service, quit)) {
			// input processed; update display
			render();
			//drawHelp();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}

void ConsoleUI::render() const {
	// Clear the console (Windows)
	std::system("cls");

	for (int i = 0; i < service.getRows(); ++i) {
		for (int j = 0; j < service.getCols(); ++j) {
			service.setCell(i, j, 1);
			bool alive = service.getCell(i, j);
			std::cout << (alive ? '1' : '0');
		}
		std::cout << '\n';
	}
	drawHelp();
}

void ConsoleUI::drawHelp() const {
	std::cout << "Controls: Space=start/pause  s=step  r=reset  e=edit r c s  1/2/3=size (S/N/L)  c=toggle rule  +/-=speed  q=quit\n";
	std::cout << "Tick(ms): " << service.getTickMs() << "  Running: " << (service.isRunning() ? "Yes" : "No") << "\n";
}


