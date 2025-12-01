#pragma once
#include "Grid.h"

class GameRules {
public:
    virtual ~GameRules() = default;
    // compute next state for position (r,c) based on `src` grid
    virtual bool computeNextState(const Grid &src, int r, int c) const = 0;
};

// Concrete implementation: Conway's Game of Life
class ConwayRules : public GameRules {
public:
    bool computeNextState(const Grid &src, int r, int c) const override;
};
