#include "Grid.h"
#include <iostream>

// constructeur
Grid::Grid(int r, int c) : rows(r), cols(c) {
    cells = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}

// destructeur
Grid::~Grid() {}

// Getters
int Grid::getR() const { return rows; }
int Grid::getC() const { return cols; }

// Setters
void Grid::setR(int r) { rows = r; }
void Grid::setC(int c) { cols = c; }

// obtenir état d'une cellule
bool Grid::getCell(int x, int y) const {
    return cells[x][y];
}

// modifier état d'une cellule
void Grid::setCell(int x, int y, bool state) {
    cells[x][y] = state;
}

// affichage console
void Grid::print() const {
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            std::cout << (cells[i][j] ? "1 " : "0 ");
        }
        std::cout << "\n";
    }
}