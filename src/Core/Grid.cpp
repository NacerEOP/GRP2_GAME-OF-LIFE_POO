#include "Grid.h"
#include <iostream>


Grid::Grid(int r, int c) : rows(r), cols(c) {
    cells = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}


Grid::~Grid() {}


int Grid::getR() const { return rows; }
int Grid::getC() const { return cols; }

void Grid::setR(int r) { rows = r; }
void Grid::setC(int c) { cols = c; }


bool Grid::getCell(int x, int y) const {
    return cells[x][y];
}


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