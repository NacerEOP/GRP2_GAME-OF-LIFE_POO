#include "ConsoleUI.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h> // _kbhit, _getch (Windows)
#include <cstdlib> // system("cls")

ConsoleUI::ConsoleUI(GameService& svc) : service(svc) {}

ConsoleUI::~ConsoleUI() = default;

void ConsoleUI::run() {
	bool quit = false;
	using clock = std::chrono::steady_clock;
	auto last = clock::now();

	drawHelp();
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

		if (_kbhit()) {
			int ch = _getch();

			switch (ch) {
			case 'q':
			case 'Q':
				quit = true;
				break;
			case ' ': // toggle start/pause
				if (service.isRunning()) service.pause();
				else service.start();
				break;
			case 's':
			case 'S':
				service.step();
				render();
				break;
			case 'r':
			case 'R':
				service.reset();
				render();
				break;
			case '1':
				service.setGridSize(GridSize::SMALL);
				render();
				break;
			case '2':
				service.setGridSize(GridSize::NORMAL);
				render();
				break;
			case '3':
				service.setGridSize(GridSize::LARGE);
				render();
				break;
			case 'c':
			case 'C': {
				// toggle rule type if possible
				RuleType rt = service.getRuleType();
				service.setRuleType(rt == RuleType::CONWAY ? RuleType::BASIC : RuleType::CONWAY);
				break;
			}
			case '+': {
				int ms = service.getTickMs();
				if (ms > 10) service.setTickMs(ms - 10);
				break;
			}
			case '-': {
				int ms = service.getTickMs();
				service.setTickMs(ms + 10);
				break;
			}
			default:
				break;
			}
			drawHelp();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}

void ConsoleUI::render() const {
	// Clear the console (Windows)
	std::system("cls");

	const Grid& g = service.getGrid();
	for (int i = 0; i < g.getR(); ++i) {
		for (int j = 0; j < g.getC(); ++j) {
			bool alive = g.getCell(i, j);
			// using simple characters for terminal
			std::cout << (alive ? 'O' : '.');
		}
		std::cout << '\n';
	}
}

void ConsoleUI::drawHelp() const {
	std::cout << "Controls: Space=start/pause  s=step  r=reset  1/2/3=size (S/N/L)  c=toggle rule  +/-=speed  q=quit\n";
	std::cout << "Tick(ms): " << service.getTickMs() << "  Running: " << (service.isRunning() ? "Yes" : "No") << "\n";
}

