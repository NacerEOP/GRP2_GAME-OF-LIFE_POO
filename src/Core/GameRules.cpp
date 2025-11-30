#include "GameRules.h"


bool ConwayRule::computeNextState(const Cell& cell, int aliveNeighbors) const
{
    bool current = cell.getStatus();

    if (!current) {
        // Une cellule morte avec exactement 3 voisines vivantes devient vivante
        return (aliveNeighbors == 3);
    }
    else {
        // Une vivant reste vivante si elle a 2 ou 3 voisines
        return (aliveNeighbors == 2 || aliveNeighbors == 3);
    }
}

