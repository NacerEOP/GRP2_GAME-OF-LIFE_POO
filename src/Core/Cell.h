#pragma once
#include <string>

class Cell {
public:
    enum class Type {
        Normal,
        Obstacle
    };

    Cell(bool alive = false, Type t = Type::Normal) : alive_(alive), type_(t) {}
    bool isAlive() const { return alive_; }
    void setAlive(bool a) { if(type_ != Type::Obstacle) alive_ = a; }
    Type getType() const { return type_; }
    void setType(Type t) { type_ = t; }
    std::string toString() const { return isAlive() ? "1" : "0"; }

private:
    bool alive_;
    Type type_;
};
