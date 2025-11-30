
#pragma once

#include <iostream> 


class Cell
{
protected:
    int PositionX;
    int PositionY;
    bool Status; // true = alive, false = dead 
    bool IsActive; // true = active, false = inactive

public:
    Cell(int x, int y);
    virtual ~Cell();
    
    // Getters
    bool getStatus() const;
    bool getIsActive() const;
    int getPositionX() const;
    int getPositionY() const;
    
    // Setters
    void setStatus(bool newStatus);
    void setIsActive(bool newIsActive);
    void setPosition(int x, int y);
    
    // Pure virtual method to make Cell abstract
    virtual void display() const = 0;
};


class AliveCell : public Cell
{
public:
    AliveCell(int x, int y);
    virtual ~AliveCell();
    void display() const override;
};


class DeadCell : public Cell
{
public:
    DeadCell(int x, int y);
    virtual ~DeadCell();
    void display() const override;
};


class AliveObstacleCell : public Cell
{
public:
    AliveObstacleCell(int x, int y);
    virtual ~AliveObstacleCell();
    void display() const override;
};


class DeadObstacleCell : public Cell
{
public:
    DeadObstacleCell(int x, int y);
    virtual ~DeadObstacleCell();
    void display() const override;
};
