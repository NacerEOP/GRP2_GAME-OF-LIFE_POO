#pragma once

#include <vector>

// Énumération pour les trois tailles de grille prédéfinies
enum class GridSize {
    SMALL,    // Petite grille: 10x10
    NORMAL,   // Grille normale: 20x20 (par défaut)
    LARGE     // Grande grille: 50x50
};

class Grid {

private:

    int rows;
    int cols;
    std::vector<std::vector<bool>> cells;

    // Méthode privée pour initialiser la grille
    void initializeGrid();

public:
    
    // Constructeur par défaut (grille NORMAL 20x20)
    Grid();
    
    // Constructeur avec taille prédéfinie
    Grid(GridSize size);
    
    // Constructeur avec dimensions personnalisées
    Grid(int r, int c);
    
    ~Grid();

    int getR() const;
    int getC() const;

    void setR(int r);
    void setC(int c);
    
    // Méthode pour redimensionner la grille avec une taille prédéfinie
    void setGridSize(GridSize size);
    
    // Méthode pour redimensionner la grille avec des dimensions précises
    void setGridDimensions(int r, int c);

    bool getCell(int x, int y) const;       // retourne l'état d'une cellule
    void setCell(int x, int y, bool state); // modifie l'état d'une cellule
    void print() const;                     // affichage console pour test

};


