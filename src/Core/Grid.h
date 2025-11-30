#include <vector>
#pragma once


class Grid {

private:

    int rows;
    int cols;
    std::vector<std::vector<bool>> cells;

public:
    
    Grid(int r, int c);
    ~Grid();

    int getR() const;
    int getC() const;

    void setR(int r);
    void setC(int c);

    bool getCell(int x, int y) const;       // retourne l'état d'une cellule
    void setCell(int x, int y, bool state); // modifie l'état d'une cellule
    void print() const;                     // affichage console pour test

};


