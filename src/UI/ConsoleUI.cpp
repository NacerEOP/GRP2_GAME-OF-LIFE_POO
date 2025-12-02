#include "ConsoleUI.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib> // system("cls")
#include <vector>
#include <string>
#include <limits>
#include "../Input/ConsoleInput.h"

ConsoleUI::ConsoleUI(GameService& svc) : service(svc) {}

ConsoleUI::~ConsoleUI() = default;

void ConsoleUI::run() {
	bool quit = false;
	using clock = std::chrono::steady_clock;
	auto last = clock::now();

	// At startup, check Input folder for .txt files and allow the user to pick one
	// Ask whether to enable toric (wrap-around) behavior
	{
		std::string resp;
		std::cout << "Enable toric wrap-around? (y/N): ";
		std::getline(std::cin, resp);
		if (!resp.empty() && (resp[0] == 'y' || resp[0] == 'Y')) service.setToric(true);
		else service.setToric(false);
	}

	// At startup, check Input folder for .txt files and allow the user to pick one
	auto files = service.listInputFiles();
	if (!files.empty()) {
		std::cout << "Found input files:\n";
		for (size_t i = 0; i < files.size(); ++i) std::cout << "  " << (i+1) << ") " << files[i] << "\n";
		std::cout << "Choose a file by number (0 = skip): ";
		int choice = -1;
		while (!(std::cin >> choice) || choice < 0 || choice > (int)files.size()) {
			std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid choice. Enter a number between 0 and " << files.size() << ": ";
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (choice > 0) {
			if (service.loadInitialFromFile(files[choice-1])) {
				std::cout << "Loaded " << files[choice-1] << "\n";
			} else {
				std::cout << "Failed to load file.\n";
			}
		}

		// ask for iteration target
		std::cout << "Enter number of iterations (0 = infinite): ";
		int it = -1;
		while (!(std::cin >> it) || it < 0) {
			std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid number. Enter 0 or a positive integer: ";
		}
		service.setIterationTarget(it);
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// ask for delay (tick ms)
		std::cout << "Enter delay between iterations in ms (default " << service.getTickMs() << "): ";
		int delay = -1;
		while (!(std::cin >> delay) || delay < 0) {
			std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid number. Enter 0 or a positive integer for ms: ";
		}
		service.setTickMs(delay);
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Use the same iteration count as the number of iterations to save
		// (0 = infinite run, and 0 means save none)
		service.setSaveIterations(it);
	}

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
			bool alive = service.getCell(i, j);
			std::cout << (alive ? '1' : '0');
		}
		std::cout << '\n';
	}
	drawHelp();
}

void ConsoleUI::drawHelp() const {
	std::cout << "Controls: Space=start/pause  s=step  r=reset  0-9=load preset  1/2/3=size (S/N/L)  c=toggle rule  +/-=speed  q=quit\n";
	std::cout << "Tick(ms): " << service.getTickMs() << "  Running: " << (service.isRunning() ? "Yes" : "No") << "\n";
}


