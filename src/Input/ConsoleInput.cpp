#include "ConsoleInput.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
// ================== WINDOWS ==================
#include <conio.h>

static bool portable_kbhit() { return _kbhit(); }
static int  portable_getch() { return _getch(); }

#else
// ================== MAC / LINUX ==================
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

static bool portable_kbhit() {
    termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}

static int portable_getch() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

#endif
// ============ FIN DU CODE PORTABLE ============


bool ConsoleInput::handleInput(GameService &service, bool &quit) {
    if (!portable_kbhit()) 
        return false;

    int ch = portable_getch();

    switch (ch) {
        case 'q': case 'Q':
            quit = true; return true;

        case ' ':
            if (service.isRunning()) service.pause();
            else service.start();
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

        case '0': case '4': case '5':
        case '6': case '7': case '8': case '9': {
            int slot = ch - '0';
            service.loadPreset(slot);
            return true;
        }

        case 'c': case 'C': {
            auto rt = service.getRuleType();
            service.setRuleType(
                rt == GameService::RuleType::CONWAY
                    ? GameService::RuleType::BASIC
                    : GameService::RuleType::CONWAY
            );
            return true;
        }

        case '+': {
            int ms = service.getTickMs();
            if (ms > 10) service.setTickMs(ms - 10);
            return true;
        }

        case '-': {
            int ms = service.getTickMs();
            service.setTickMs(ms + 10);
            return true;
        }

        default:
            return false;
    }
}