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
    virtual std::string toString() const = 0;

    // Factories
    static std::unique_ptr<Cell> createDefault(bool alive = false, Type t = Type::Normal);
    static std::unique_ptr<Cell> createAlive();
    static std::unique_ptr<Cell> createDead();
    static std::unique_ptr<Cell> createObstacle(bool alive = false);
};

// Concrete subclasses declared for clarity (definitions in Cell.cpp)
class AliveCell;
class DeadCell;
class ObstacleCell;



