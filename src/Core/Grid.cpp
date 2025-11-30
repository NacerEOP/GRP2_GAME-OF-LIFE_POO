#include "Grid.h"

Grid::Grid(int r, int c) : rows(r), cols(c) {
    
}

Grid::~Grid() {

}

int Grid::getR() const {
    return r;
}

int Grid::getC() const {
    return c;
}

void Grid::setR(int r) {
    this->r = r;
}

void Grid::setC(int c) {
    this->c = c;
}




