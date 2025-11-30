
#include "GameRules.h"


bool ConwayRule::computeNextState(const Cell& cell, const Grid& grid) const
{
    int aliveNeighbors = countAlive(cell, grid);  // ← appelle MooreNeighborhood automatiquement
    bool current = cell.getStatus();

    if (!current) {
        return (aliveNeighbors == 3);
    } else {
        return (aliveNeighbors == 2 || aliveNeighbors == 3);
    }
}


bool BasicRule::computeNextState(const Cell& cell, const Grid& grid) const
{
    int aliveNeighbors = countAlive(cell, grid);  // ← appelle VonNeumannNeighborhood automatiquement

    // Exemple simple (à modifier selon ta règle)
    return (aliveNeighbors >= 1);
}

