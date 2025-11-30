
#pragma once
#include <iostream>

#include "Cell.h"
#include "Grid.h"

// Abstract neighborhood interface
class Neighborhood {
public:
    virtual ~Neighborhood() = default;
    virtual int countAliveNeighbors(const Cell& cell, const Grid& grid) const = 0;
};

// Concrete neighborhoods
namespace Neighborhoods {
    class MooreNeighborhood : public Neighborhood {
    public:
        int countAliveNeighbors(const Cell& cell, const Grid& grid) const override;
    };

    class VonNeumannNeighborhood : public Neighborhood {
    public:
        int countAliveNeighbors(const Cell& cell, const Grid& grid) const override;
    };
}

// Abstract rule base class
class Rule {
protected:
    Neighborhood* neighborhood;

public:
    explicit Rule(Neighborhood* n) : neighborhood(n) {}
    virtual ~Rule() = default;

    // Retourne le futur état
    virtual bool computeNextState(const Cell& cell, const Grid& grid) const = 0;

protected:
    int countAlive(const Cell& cell, const Grid& grid) const {
        return neighborhood->countAliveNeighbors(cell, grid);
    }
};

// Basic rule (example)
class BasicRule : public Rule {
public:
    BasicRule() : Rule(new Neighborhoods::VonNeumannNeighborhood()) {}
    bool computeNextState(const Cell& cell, const Grid& grid) const override;
};

// Conway's Game of Life rule
class ConwayRule : public Rule {
public:
    ConwayRule() : Rule(new Neighborhoods::MooreNeighborhood()) {}
    bool computeNextState(const Cell& cell, const Grid& grid) const override;
};

