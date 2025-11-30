#include <iostream>
#pragma once


class Grid {

private:
    int rows;
    int cols;

public:
    Grid(int r, int c) ;
    ~Grid();
    int getR() const;
    int getC() const;
    void setR(int r);
    void setC(int c);

};


