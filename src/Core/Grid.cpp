
#include "Grid.h"
#include <iostream>

// helper to allocate grid storage
static std::vector<std::vector<std::unique_ptr<Cell>>> makeCellMatrix(int rows, int cols) {
    std::vector<std::vector<std::unique_ptr<Cell>>> m;
    m.resize(rows);
    for (int r = 0; r < rows; ++r) {
        m[r].resize(cols);
        for (int c = 0; c < cols; ++c) {
            m[r][c] = Cell::createDead();
        }
    }
    return m;
}

// Constructeur par défaut (grille NORMAL 20x20)
Grid::Grid() : rows(20), cols(20), cells(makeCellMatrix(20,20)), toric(false) {}

// Constructeur avec taille prédéfinie
Grid::Grid(GridSize size) : toric(false) {
    switch (size) {
        case GridSize::SMALL: rows = 10; cols = 10; break;
        case GridSize::NORMAL: rows = 20; cols = 20; break;
        case GridSize::LARGE: rows = 50; cols = 50; break;
        default: rows = 20; cols = 20; break;
    }
    cells = makeCellMatrix(rows, cols);
}

// Constructeur avec dimensions personnalisées
Grid::Grid(int r, int c) : rows(r), cols(c), cells(makeCellMatrix(r,c)), toric(false) {}

Grid::~Grid() {}

int Grid::getR() const { return rows; }
int Grid::getC() const { return cols; }

void Grid::setR(int r) { rows = r; cells = makeCellMatrix(rows, cols); }
void Grid::setC(int c) { cols = c; cells = makeCellMatrix(rows, cols); }

// Redimensionner avec une taille prédéfinie
void Grid::setGridSize(GridSize size) {
    switch (size) {
        case GridSize::SMALL: rows = 10; cols = 10; break;
        case GridSize::NORMAL: rows = 20; cols = 20; break;
        case GridSize::LARGE: rows = 50; cols = 50; break;
        default: rows = 20; cols = 20; break;
    }
    cells = makeCellMatrix(rows, cols);
}

// Redimensionner avec des dimensions précises
void Grid::setGridDimensions(int r, int c) {
    rows = r; cols = c; cells = makeCellMatrix(rows, cols);
}

// copy ctor / assignment (deep copy)
Grid::Grid(const Grid &other) : rows(other.rows), cols(other.cols), toric(other.toric) {
    cells = makeCellMatrix(rows, cols);
    for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) {
        bool alive = other.getCell(r,c);
        if (other.isObstacle(r,c)) cells[r][c] = Cell::createObstacle(alive);
        else cells[r][c] = Cell::createDefault(alive, Cell::Type::Normal);
    }
}

Grid& Grid::operator=(const Grid &other) {
    if (this == &other) return *this;
    rows = other.rows; cols = other.cols; toric = other.toric;
    cells = makeCellMatrix(rows, cols);
    for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) {
        bool alive = other.getCell(r,c);
        if (other.isObstacle(r,c)) cells[r][c] = Cell::createObstacle(alive);
        else cells[r][c] = Cell::createDefault(alive, Cell::Type::Normal);
    }
    return *this;
}

bool Grid::getCell(int x, int y) const {
    if (rows <= 0 || cols <= 0) return false;
    if (toric) {
        int rx = ((x % rows) + rows) % rows;
        int ry = ((y % cols) + cols) % cols;
        return cells[rx][ry]->isAlive();
    }
    if (x < 0 || x >= rows || y < 0 || y >= cols) throw std::out_of_range("Grid::getCell index out of range");
    return cells[x][y]->isAlive();
}

void Grid::setCell(int x, int y, bool state) {
    if (toric) {
        int rx = ((x % rows) + rows) % rows;
        int ry = ((y % cols) + cols) % cols;
        x = rx; y = ry;
    }
    if (x < 0 || x >= rows || y < 0 || y >= cols) throw std::out_of_range("Grid::setCell index out of range");
    // if obstacle, modify its alive flag
    if (cells[x][y]->getType() == Cell::Type::Obstacle) {
        cells[x][y]->setAlive(state);
        return;
    }
    // replace the cell with an appropriate Alive/Dead cell
    cells[x][y] = Cell::createDefault(state, Cell::Type::Normal);
}

void Grid::setToric(bool t) { toric = t; }
bool Grid::isToric() const { return toric; }

void Grid::setObstacle(int x, int y, bool obs) {
    if (toric) {
        int rx = ((x % rows) + rows) % rows;
        int ry = ((y % cols) + cols) % cols;
        x = rx; y = ry;
    }
    if (x < 0 || x >= rows || y < 0 || y >= cols) throw std::out_of_range("Grid::setObstacle index out of range");
    bool alive = cells[x][y]->isAlive();
    if (obs) cells[x][y] = Cell::createObstacle(alive);
    else cells[x][y] = Cell::createDefault(alive, Cell::Type::Normal);
}

bool Grid::isObstacle(int x, int y) const {
    if (toric) {
        int rx = ((x % rows) + rows) % rows;
        int ry = ((y % cols) + cols) % cols;
        return cells[rx][ry]->getType() == Cell::Type::Obstacle;
    }
    if (x < 0 || x >= rows || y < 0 || y >= cols) throw std::out_of_range("Grid::isObstacle index out of range");
    return cells[x][y]->getType() == Cell::Type::Obstacle;
}

// deep equality check
bool Grid::equals(const Grid &other) const {
    if (rows != other.rows || cols != other.cols) return false;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (isObstacle(r,c) != other.isObstacle(r,c)) return false;
            if (getCell(r,c) != other.getCell(r,c)) return false;
        }
    }
    return true;
}

// affichage console
void Grid::print() const {
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if (isObstacle(i,j)) {
                std::cout << (getCell(i,j) ? "A " : "D ");
            } else {
                std::cout << (getCell(i,j) ? "1 " : "0 ");
            }
        }
        std::cout << "\n";
    }
}

