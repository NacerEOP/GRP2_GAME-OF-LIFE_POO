#pragma once
#include <string>
#include <memory>

class Cell {
public:
    enum class Type { Normal, Obstacle };
    virtual ~Cell() = default;

    // Abstract interface
    virtual bool isAlive() const = 0;
    virtual void setAlive(bool a) = 0;
    virtual Type getType() const = 0;
    virtual void setType(Type t) = 0;
    virtual std::string toString() const = 0;

    // Factory for a default concrete Cell implementation
    static std::unique_ptr<Cell> createDefault(bool alive = false, Type t = Type::Normal);
};


class Cell_Alive:Cell
{
private:
    /* data */
public:
    Cell_Alive(/* args */);
    ~Cell_Alive();
};

class Cell_Dead:Cell
{
private:
    /* data */
public:
    Cell_Dead(/* args */);
    ~Cell_Dead();
};

class Cell_Obstacle:Cell
{
private:
    /* data */
public:
    Cell_Obstacle(/* args */);
    ~Cell_Obstacle();
};



