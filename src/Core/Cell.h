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

// Concrete AliveCell - always alive (declaration only)
class AliveCell : public Cell {
public:
    AliveCell();
    ~AliveCell() override;
    bool isAlive() const override;
    void setAlive(bool a) override;
    Type getType() const override;
    std::string toString() const override;
};

// Concrete DeadCell - always dead (declaration only)
class DeadCell : public Cell {
public:
    DeadCell();
    ~DeadCell() override;
    bool isAlive() const override;
    void setAlive(bool a) override;
    Type getType() const override;
    std::string toString() const override;
};

// Concrete ObstacleCell - can be alive or dead (declaration only)
class ObstacleCell : public Cell {
public:
    explicit ObstacleCell(bool alive = false);
    ~ObstacleCell() override;
    bool isAlive() const override;
    void setAlive(bool a) override;
    Type getType() const override;
    std::string toString() const override;
private:
    bool alive_;
};



