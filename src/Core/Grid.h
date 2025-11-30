#include <iostream>
#pragma once


class Grid {
    
private:
    int x;
    int y;

public:
    Grid(int x, int y) ;
    ~Grid();
    int getX() const;
    int getY() const;
    void setX(int x);
    void setY(int y);

};


