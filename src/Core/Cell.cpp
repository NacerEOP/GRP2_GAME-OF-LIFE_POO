#include "Cell.h"

// Cell class implementation
Cell::Cell(int x, int y) : PositionX(x), PositionY(y), Status(false), IsActive(false)
{
}

Cell::~Cell()
{
}

bool Cell::getStatus() const
{
    return Status;
}

bool Cell::getIsActive() const
{
    return IsActive;
}

int Cell::getPositionX() const
{
    return PositionX;
}

int Cell::getPositionY() const
{
    return PositionY;
}

void Cell::setStatus(bool newStatus)
{
    Status = newStatus;
}

void Cell::setIsActive(bool newIsActive)
{
    IsActive = newIsActive;
}

void Cell::setPosition(int x, int y)
{
    PositionX = x;
    PositionY = y;
}


// AliveCell class implementation
AliveCell::AliveCell(int x, int y) : Cell(x, y)
{
    Status = true;  // Alive cells have Status = true
    IsActive = true; // Alive cells are active by default
}

AliveCell::~AliveCell()
{
}

void AliveCell::display() const
{
    std::cout << "Alive Cell at (" << PositionX << ", " << PositionY << ")" << std::endl;
}


// DeadCell class implementation
DeadCell::DeadCell(int x, int y) : Cell(x, y)
{
    Status = false;  // Dead cells have Status = false
    IsActive = true; // Dead cells are inactive by default
}

DeadCell::~DeadCell()
{
}

void DeadCell::display() const
{
    std::cout << "Dead Cell at (" << PositionX << ", " << PositionY << ")" << std::endl;
}


// AliveObstacleCell class implementation
AliveObstacleCell::AliveObstacleCell(int x, int y) : Cell(x, y)
{
    Status = true;  // Alive Obstacle cells have Status = true
    IsActive = false; // Alive Obstacle cells are inactive by default
}

AliveObstacleCell::~AliveObstacleCell()
{
}

void AliveObstacleCell::display() const
{
    std::cout << "Alive Obstacle Cell at (" << PositionX << ", " << PositionY << ")" << std::endl;
}


// DeadObstacleCell class implementation
DeadObstacleCell::DeadObstacleCell(int x, int y) : Cell(x, y)
{
    Status = false;  // Dead Obstacle cells have Status = false
    IsActive = false; // Dead Obstacle cells are inactive by default
}

DeadObstacleCell::~DeadObstacleCell()
{
}

void DeadObstacleCell::display() const
{
    std::cout << "Dead Obstacle Cell at (" << PositionX << ", " << PositionY << ")" << std::endl;
}
