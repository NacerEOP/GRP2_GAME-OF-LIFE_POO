
#pragma once
#include <iostream> 

#include "Cell.h"
#include "Grid.h"


class Rule
{
protected:
    Neighborhood* neighborhood;

public:
    Rule(Neighborhood* n) : neighborhood(n) {}
    virtual ~Rule() = default;

    // Retourne le futur état
    virtual bool computeNextState(const Cell& cell, const Grid& grid) const = 0;

protected:
    int countAlive(const Cell& cell, const Grid& grid) const {
        return neighborhood->countAliveNeighbors(cell, grid);
    }
};


class BasicRule : public Rule
{
public:
    BasicRule() : Rule(new Neighborhoods::VonNeumannNeighborhood()) {}

    bool computeNextState(const Cell& cell, const Grid& grid) const override;
};

class ConwayRule : public Rule
{
public:
    ConwayRule() : Rule(new Neighborhoods::MooreNeighborhood()) {}

    bool computeNextState(const Cell& cell, const Grid& grid) const override;
};





class Neighborhood
{
public:
    virtual ~Neighborhood() = default;

    // Retourne le nombre de voisins vivants autour de la cellule donnée
    virtual int countAliveNeighbors(const Cell& cell, const Grid& grid) const = 0;
};


namespace Neighborhoods {

    class MooreNeighborhood : public Neighborhood
    {
    public:
        int countAliveNeighbors(const Cell& cell, const Grid& grid) const override;
    };

    class VonNeumannNeighborhood : public Neighborhood
    {
    public:
        int countAliveNeighbors(const Cell& cell, const Grid& grid) const override;
    };

} 

