
#include "GameRules.h"


bool BasicRule::computeNextState(const Cell& cell, const Grid& grid) const
{
    int aliveNeighbors = countAlive(cell, grid);
    bool current = cell.getStatus();

    // Exemple de règle simple :
    // - Une cellule vivante meurt si aucun voisin
    // - Une cellule morte devient vivante si >= 2 voisins
    if (current)
        return (aliveNeighbors >= 1);

    return (aliveNeighbors >= 2);
}


bool ConwayRule::computeNextState(const Cell& cell, const Grid& grid) const
{
    int aliveNeighbors = countAlive(cell, grid);
    bool current = cell.getStatus();

    if (!current)
        return (aliveNeighbors == 3);   // naissance

    return (aliveNeighbors == 2 || aliveNeighbors == 3); // survie
}



namespace Neighborhoods {


    int MooreNeighborhood::countAliveNeighbors(const Cell& cell, const Grid& grid) const
    {
        static const int offsets[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
        };

        int x = cell.getPositionX();
        int y = cell.getPositionY();
        int count = 0;

        for (auto &o : offsets)
        {
            int nx = x + o[0];
            int ny = y + o[1];

            // Vérifier les limites
            if (nx >= 0 && nx < grid.getR() && ny >= 0 && ny < grid.getC())
            {
                if (grid.getCell(nx, ny))
                    count++;
            }
        }

        return count;
    }



    int VonNeumannNeighborhood::countAliveNeighbors(const Cell& cell, const Grid& grid) const
    {
        static const int offsets[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };

        int x = cell.getPositionX();
        int y = cell.getPositionY();
        int count = 0;

        for (auto &o : offsets)
        {
            int nx = x + o[0];
            int ny = y + o[1];

            // Vérifier les limites
            if (nx >= 0 && nx < grid.getR() && ny >= 0 && ny < grid.getC())
            {
                if (grid.getCell(nx, ny))
                    count++;
            }
        }

        return count;
    }

}

