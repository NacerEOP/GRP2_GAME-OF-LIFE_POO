
#pragma once


#include "../Services/GameService.h"

class ConsoleInput {
public:
    // Poll for input and handle it. If input was handled, returns true.
    // If the user requested quit, sets quit=true.
    bool handleInput(GameService &service, bool &quit);
};
