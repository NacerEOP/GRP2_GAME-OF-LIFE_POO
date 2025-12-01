#pragma once
#include "Cell.h"

class GameRules {
public:
    virtual ~GameRules() = default;
    // Compute next state given current cell and number of alive neighbors.
    virtual bool computeNextState(const Cell& current, int aliveNeighbors) const = 0;
};
