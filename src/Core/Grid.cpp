#include "Grid.h"

Grid::Grid(int x, int y) : x(x), y(y) {
    
}

Grid::~Grid() {

}

int Grid::getX() const {
    return x;
}

int Grid::getY() const {
    return y;
}

void Grid::setX(int x) {
    this->x = x;
}

void Grid::setY(int y) {
    this->y = y;
}




