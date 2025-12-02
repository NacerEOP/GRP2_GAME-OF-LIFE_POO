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
    case '1':
        service.setGridSize(GridSize::SMALL); return true;
    case '2':
        service.setGridSize(GridSize::NORMAL); return true;
    case '3':
        service.setGridSize(GridSize::LARGE); return true;
    // Preset keys: 0,4-9 (keep 1-3 reserved for sizes)
    case '0': case '4': case '5': case '6': case '7': case '8': case '9': {
        int slot = ch - '0';
        service.loadPreset(slot);
        return true;
    }
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
