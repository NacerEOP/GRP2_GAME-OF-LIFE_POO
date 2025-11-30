
#include "Grid.h"
#include <iostream>

// Méthode privée pour initialiser la grille avec les dimensions actuelles
void Grid::initializeGrid() {
    cells = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}

// Constructeur par défaut (grille NORMAL 20x20)
Grid::Grid() : rows(20), cols(20) {
    initializeGrid();
}

// Constructeur avec taille prédéfinie
Grid::Grid(GridSize size) {
    switch (size) {
        case GridSize::SMALL:
            rows = 10;
            cols = 10;
            break;
        case GridSize::NORMAL:
            rows = 20;
            cols = 20;
            break;
        case GridSize::LARGE:
            rows = 50;
            cols = 50;
            break;
        default:
            rows = 20;
            cols = 20;
    }
    initializeGrid();
}

// Constructeur avec dimensions personnalisées
Grid::Grid(int r, int c) : rows(r), cols(c) {
    initializeGrid();
}

Grid::~Grid() {}

int Grid::getR() const { return rows; }
int Grid::getC() const { return cols; }

void Grid::setR(int r) { 
    rows = r;
    initializeGrid();
}

void Grid::setC(int c) { 
    cols = c;
    initializeGrid();
}

// Redimensionner avec une taille prédéfinie
void Grid::setGridSize(GridSize size) {
    switch (size) {
        case GridSize::SMALL:
            rows = 10;
            cols = 10;
            break;
        case GridSize::NORMAL:
            rows = 20;
            cols = 20;
            break;
        case GridSize::LARGE:
            rows = 50;
            cols = 50;
            break;
        default:
            rows = 20;
            cols = 20;
    }
    initializeGrid();
}

// Redimensionner avec des dimensions précises
void Grid::setGridDimensions(int r, int c) {
    rows = r;
    cols = c;
    initializeGrid();
}

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

