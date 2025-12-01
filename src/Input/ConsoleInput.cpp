#include "ConsoleInput.h"

#include <iostream>
#include <conio.h>
#include <sstream>

bool ConsoleInput::handleInput(GameService &service, bool &quit) {
    if (!_kbhit()) return false;
    int ch = _getch();

    switch (ch) {
    case 'q': case 'Q':
        quit = true; return true;
    case ' ': // toggle start/pause
        if (service.isRunning()) service.pause(); else service.start();
        return true;
    case 's': case 'S':
        service.step(); return true;
    case 'r': case 'R':
        service.reset(); return true;
    case 'e': case 'E': {
        // Edit a single cell: prompt for row col state(0/1)
        std::cin.clear();
        std::string line;
        std::cout << "Enter row col state(0/1), e.g. 3 4 1: ";
        std::getline(std::cin, line);
        if (line.empty()) std::getline(std::cin, line);
        std::istringstream iss(line);
        int r = -1, c = -1; int s = 0;
        if ((iss >> r >> c >> s)) {
            if (r >= 0 && r < service.getRows() && c >= 0 && c < service.getCols()) {
                service.setCell(r, c, s != 0);
            } else {
                std::cout << "Invalid coordinates\n";
            }
        } else {
            std::cout << "Invalid input\n";
        }
        return true;
    }
    case '1':
        service.setGridSize(GridSize::SMALL); return true;
    case '2':
        service.setGridSize(GridSize::NORMAL); return true;
    case '3':
        service.setGridSize(GridSize::LARGE); return true;
    case 'c': case 'C': {
        auto rt = service.getRuleType();
        service.setRuleType(rt == GameService::RuleType::CONWAY ? GameService::RuleType::BASIC : GameService::RuleType::CONWAY);
        return true;
    }
    case '+': {
        int ms = service.getTickMs(); if (ms > 10) service.setTickMs(ms - 10); return true;
    }
    case '-': {
        int ms = service.getTickMs(); service.setTickMs(ms + 10); return true;
    }
    default:
        return false;
    }
}
